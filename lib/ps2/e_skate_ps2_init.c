#include <e_skate_err.h>
#include <e_skate_config.h>
#include <e_skate_ps2.h>
#include <e_skate_ps2_utils.h>

#include <esp_intr_alloc.h>
#include <driver/gpio.h>
#include <driver/timer.h>


void IRAM_ATTR e_skate_ps2_isr(

    void* param

)
{
    e_skate_ps2_handle_t*   ps2Handle = (e_skate_ps2_handle_t*) param;
    e_skate_ps2_config_t*   ps2Config = (e_skate_ps2_config_t*) &( ps2Handle->ps2Config );
    e_skate_ps2_bit_t       newBit;

    newBit.bit = gpio_get_level(
        ps2Config->gpioConfig.dataPin);

    /* Get the time since the last bit */
    timer_get_counter_time_sec(
        ps2Config->timerConfig.timerGroup,
        ps2Config->timerConfig.timerIdx,
        &newBit.bitInterval_s);

    /* Reset Timer */
    timer_set_counter_value(
        ps2Config->timerConfig.timerGroup,
        ps2Config->timerConfig.timerIdx,
        (uint64_t) 0);

    xQueueSendFromISR(
        ps2Handle->rxBitQueueHandle,
        &newBit,
        NULL);
}


/* Infinite Task */
void ps2_rx_consumer_task(

    void* param

) 
{
    e_skate_ps2_handle_t*   ps2Handle = (e_skate_ps2_handle_t*) param;
    e_skate_ps2_pkt_t*      ps2Pkt    = &ps2Handle->rxPkt;
    e_skate_ps2_bit_t       newData;

    while(true)
    {
        if(!xQueueReceive(ps2Handle->rxBitQueueHandle, &newData, portMAX_DELAY))
        {
            e_skate_ps2_reset_pkt(ps2Pkt);
            continue;
        }

        if (newData.bitInterval_s > (double) E_SKATE_PS2_PACKET_TIMEOUT_MS / 1000)
            e_skate_ps2_reset_pkt(ps2Pkt);

        if (e_skate_ps2_add_bit(ps2Pkt, newData.bit) == E_SKATE_PS2_ERR_VALUE_READY)
        {
            if (e_skate_ps2_check_pkt(ps2Pkt) == E_SKATE_SUCCESS)
                xQueueSend(ps2Handle->rxByteQueueHandle, &ps2Pkt->newByte, 0);
            e_skate_ps2_reset_pkt(ps2Pkt);
        }
    }
}


e_skate_err_t e_skate_ps2_init(

    e_skate_ps2_handle_t* ps2Handle,
    e_skate_ps2_config_t* ps2Config

)
{
    ps2Handle->ps2Config = *ps2Config;
    timer_config_t espTimerConf = {
        .divider        = 16,
        .counter_dir    = TIMER_COUNT_UP    ,
        .counter_en     = TIMER_START       ,
        .alarm_en       = TIMER_ALARM_DIS   ,
        .intr_type      = TIMER_INTR_LEVEL  ,
        .auto_reload    = false
    };
    esp_err_t isrErrCode;


    ps2Handle->rxBitQueueHandle  = xQueueCreate(11 /* Frame length */, sizeof(e_skate_ps2_bit_t));
    ps2Handle->rxByteQueueHandle = xQueueCreate(E_SKATE_PS2_BYTE_QUEUE_LENGTH, 1);
    ps2Handle->txByteQueueHandle = xQueueCreate(E_SKATE_PS2_BYTE_QUEUE_LENGTH, 1);

    if (xTaskCreate(
            ps2_rx_consumer_task,
            "ps2_rx_consumer_task",
            2048, ps2Handle,
            E_SKATE_PS2_RX_TASK_PRIORITY,
            &ps2Handle->rxTaskHandle) != pdPASS)
    {
        ps2Handle->rxTaskHandle = NULL;
    }
    
    // TODO Tx task

    if  (
            ps2Handle->rxBitQueueHandle  == NULL ||
            ps2Handle->rxByteQueueHandle == NULL ||
            ps2Handle->txByteQueueHandle == NULL ||
            ps2Handle->rxTaskHandle      == NULL
        )
    {
        e_skate_ps2_deinit(ps2Handle, false);
        return E_SKATE_ERR_OOM;
    }

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

    isrErrCode = gpio_install_isr_service(ESP_INTR_FLAG_IRAM);

    /* Service might already be installed. */
    if  (isrErrCode != ESP_OK && isrErrCode != ESP_ERR_INVALID_STATE)
        ESP_ERROR_CHECK(isrErrCode);

    ESP_ERROR_CHECK(gpio_isr_handler_add(
        ps2Config->gpioConfig.clockPin,
        e_skate_ps2_isr,
        ps2Handle));

    /* Timer Init */
    ESP_ERROR_CHECK(timer_init(
        ps2Config->timerConfig.timerGroup,
        ps2Config->timerConfig.timerIdx,
        &espTimerConf));

    e_skate_ps2_reset_pkt(&ps2Handle->rxPkt);
    e_skate_ps2_reset_pkt(&ps2Handle->txPkt);

    return E_SKATE_SUCCESS;
}