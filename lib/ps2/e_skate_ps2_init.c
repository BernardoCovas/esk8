#include <e_skate_err.h>
#include <e_skate_config.h>
#include <e_skate_ps2.h>
#include <e_skate_ps2_utils.h>

#include <esp_intr_alloc.h>
#include <driver/gpio.h>
#include <driver/timer.h>


void IRAM_ATTR e_skate_ps2_isr(void* param)
{
    e_skate_ps2_handle_t* ps2Handle = (e_skate_ps2_handle_t*) param;
    e_skate_ps2_config_t* ps2Config = (e_skate_ps2_config_t*) &( ps2Handle->ps2Config );

    bool newBit = gpio_get_level(
        ps2Config->gpioConfig.dataPin);

    /**
     * NOTE (b.covas): 100 packets
     * per second.
     * If around 5/6 ms have passed
     * since the last clock,
     * reset and clear the values.
     **/
    uint64_t currBitTime;

    timer_get_counter_value(
        ps2Config->timerConfig.timerGroup,
        ps2Config->timerConfig.timerIdx,
        &currBitTime);

    int     timerScale = TIMER_BASE_CLK / (ps2Handle->ps2Config.timerConfig.timerDivider * 1000);
    uint64_t pktDiffMs = currBitTime    / timerScale;

    /* Reset Counter */
    timer_set_counter_value(
        ps2Config->timerConfig.timerGroup,
        ps2Config->timerConfig.timerIdx,
        (uint64_t) 0);

    /**
     * If too much time passed since the last bit,
     * it's a new packet.
     **/
    if (pktDiffMs > E_SKATE_PS2_PACKET_TIMEOUT_MS)
        e_skate_ps2_reset_handle(
            ps2Handle,
            true);

    e_skate_err_t bitErrCode = e_skate_ps2_add_bit(
        ps2Handle,
        newBit);

    if (bitErrCode == E_SKATE_PS2_ERR_VALUE_READY)
    {
        // Send the new byte to the queue
        xQueueSendFromISR(
            ps2Handle->byteQueueHandle,
            &( ps2Handle->newByte ),
            NULL);

        e_skate_ps2_reset_handle(
            ps2Handle,
            true);
    }
}


e_skate_err_t e_skate_ps2_init(

    e_skate_ps2_handle_t* ps2Handle,
    e_skate_ps2_config_t* ps2Config

)
{
    ps2Handle->ps2Config = *ps2Config;

    ps2Handle->byteQueueHandle = xQueueCreate(
        E_SKATE_PS2_BYTE_QUEUE_LENGTH,
        1);

    if (ps2Handle->byteQueueHandle == NULL)
        return E_SKATE_ERR_OOM;

    /* GPIO Interrupt Init */
    ESP_ERROR_CHECK(gpio_set_pull_mode(
        ps2Config->gpioConfig.clockPin,
        GPIO_PULLUP_ONLY));

    ESP_ERROR_CHECK(gpio_set_pull_mode(
        ps2Config->gpioConfig.dataPin,
        GPIO_PULLUP_ONLY));
   
    ESP_ERROR_CHECK(gpio_set_direction(
        ps2Config->gpioConfig.dataPin,
        GPIO_MODE_INPUT));

    ESP_ERROR_CHECK(gpio_set_direction(
        ps2Config->gpioConfig.clockPin,
        GPIO_MODE_INPUT));

    /* Falling edge intr, as per Ps2. */
    ESP_ERROR_CHECK(gpio_set_intr_type(
        ps2Config->gpioConfig.clockPin,
        GPIO_INTR_NEGEDGE));

    esp_err_t isrErrCode = gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    
    /* Service might already be installed. */
    if  (isrErrCode != ESP_OK && isrErrCode != ESP_ERR_INVALID_STATE)
        ESP_ERROR_CHECK(isrErrCode);

    ESP_ERROR_CHECK(gpio_isr_handler_add(
        ps2Config->gpioConfig.clockPin,
        e_skate_ps2_isr,
        ps2Handle));


    timer_config_t espTimerConf;
    espTimerConf.divider        = ps2Config->timerConfig.timerDivider;
    espTimerConf.counter_dir    = TIMER_COUNT_UP;
    espTimerConf.counter_en     = TIMER_START;
    espTimerConf.alarm_en       = TIMER_ALARM_DIS;
    espTimerConf.intr_type      = TIMER_INTR_LEVEL;
    espTimerConf.auto_reload    = false;

    /* Timer Init */
    ESP_ERROR_CHECK(timer_init(
        ps2Config->timerConfig.timerGroup,
        ps2Config->timerConfig.timerIdx,
        &espTimerConf));

    e_skate_ps2_reset_handle(ps2Handle, true);

    return E_SKATE_SUCCESS;
}