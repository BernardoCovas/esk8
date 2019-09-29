#include <esk8_err.h>
#include <esk8_config.h>
#include <esk8_ps2.h>
#include <esk8_ps2_utils.h>

#include <esp_intr_alloc.h>
#include <driver/gpio.h>
#include <driver/timer.h>


void esk8_ps2_isr(

    void* param

)
{
    esk8_ps2_handle_t*      ps2Handle   = (esk8_ps2_handle_t*) param;
    esk8_ps2_config_t*      ps2Config   = &(ps2Handle->ps2Config);
    esk8_ps2_pkt_t*         ps2Pkt      = &ps2Handle->rxPkt;

    timer_group_t tg = ps2Config->timerConfig.timerGroup;
    timer_idx_t   ti = ps2Config->timerConfig.timerIdx;
    gpio_num_t d_pin = ps2Config->gpioConfig.dataPin;
    gpio_num_t c_pin = ps2Config->gpioConfig.clockPin;

    if (ps2Config->dataDrctn == PS2_DIRCN_RECV)
    {
        int data;
        double intSec;

        data = gpio_get_level(d_pin);                                   /* NOTE (b.covas): Measure the data pin right away  */
        timer_get_counter_time_sec(tg, ti, &intSec);                    /* NOTE (b.covas): Get the time since the last bit  */
        timer_set_counter_value(tg, ti, (uint64_t) 0);                  /* NOTE (b.covas): Reset Timer                      */

        if (intSec > (double) (ESK8_PS2_PACKET_TIMEOUT_uS / (1E6)))     /* NOTE (b.covas): If the bit took too long, reset  */
            esk8_ps2_reset_pkt(ps2Pkt);

        if (esk8_ps2_add_bit(ps2Pkt, data) == ESK8_PS2_ERR_VALUE_READY)
        {
            if (esk8_ps2_check_pkt(ps2Pkt) == ESK8_SUCCESS)
                // NOTE (b.covas): If this is not true, we lost a packet.
                /* NOTE (b.covas): Send to queue */
                xQueueSendFromISR(
                    ps2Handle->rxByteQueueHandle,
                    &ps2Pkt->newByte,
                    NULL
                );

            esk8_ps2_reset_pkt(&ps2Handle->txPkt);
            esk8_ps2_reset_pkt(&ps2Handle->rxPkt);
        }

        return;
    }

    if (ps2Handle->ps2Config.dataDrctn == PS2_DIRCN_SEND)
    {
        if (ps2Handle->txTaskToNotift == NULL)
            return;

        bool bitVal;
        esk8_err_t errCode = esk8_ps2_take_bit(&ps2Handle->txPkt, &bitVal);

        if (errCode == ESK8_PS2_ERR_VALUE_READY)
        {
            ps2Config->dataDrctn = PS2_DIRCN_RECV;

            gpio_set_direction(d_pin, GPIO_MODE_INPUT);
            gpio_set_intr_type(c_pin, GPIO_INTR_NEGEDGE);
            vTaskNotifyGiveFromISR(ps2Handle->txTaskToNotift, NULL);

            esk8_ps2_reset_pkt(&ps2Handle->txPkt);
            esk8_ps2_reset_pkt(&ps2Handle->rxPkt);

            return;
        }

        // 5 us minimum before changing the data pin state
        vTaskDelay(5 / portTICK_PERIOD_MS / 1000);
        gpio_set_level(d_pin, (uint32_t) bitVal);
    }
}


esk8_err_t esk8_ps2_init(

    esk8_ps2_handle_t* ps2Handle,
    esk8_ps2_config_t* ps2Config

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

    ps2Handle->rxByteQueueHandle  = xQueueCreate(ESK8_PS2_BYTE_QUEUE_LENGTH , sizeof(uint8_t));

    if  (ps2Handle->rxByteQueueHandle == NULL)
    {
        esk8_ps2_deinit(ps2Handle, false);
        return ESK8_ERR_OOM;
    }

    /* NOTE (b.covas): GPIO Interrupt Init */
    gpio_num_t d_pin, c_pin;
    d_pin = ps2Config->gpioConfig.dataPin;
    c_pin = ps2Config->gpioConfig.clockPin;

    ESP_ERROR_CHECK(gpio_set_pull_mode(c_pin, GPIO_PULLUP_ONLY));
    ESP_ERROR_CHECK(gpio_set_pull_mode(d_pin, GPIO_PULLUP_ONLY));
    ESP_ERROR_CHECK(gpio_set_drive_capability(c_pin, GPIO_DRIVE_CAP_0));
    ESP_ERROR_CHECK(gpio_set_drive_capability(d_pin, GPIO_DRIVE_CAP_0));
    ESP_ERROR_CHECK(gpio_set_direction(c_pin, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(d_pin, GPIO_MODE_INPUT));

    /* NOTE (b.covas): Falling edge intr, as per Ps2. */
    ESP_ERROR_CHECK(gpio_set_intr_type(c_pin, GPIO_INTR_NEGEDGE));

    esp_err_t isrErrCode = gpio_install_isr_service(0);

    /* NOTE (b.covas): Service might already be installed. */
    if  (isrErrCode != ESP_OK && isrErrCode != ESP_ERR_INVALID_STATE)
        ESP_ERROR_CHECK(isrErrCode);

    ESP_ERROR_CHECK(gpio_isr_handler_add(c_pin, esk8_ps2_isr, ps2Handle));

    /* NOTE (b.covas): Timer Init */
    ESP_ERROR_CHECK(timer_init(
        ps2Config->timerConfig.timerGroup,
        ps2Config->timerConfig.timerIdx,
        &espTimerConf));

    esk8_ps2_reset_pkt(&ps2Handle->rxPkt);
    esk8_ps2_reset_pkt(&ps2Handle->txPkt);

    return ESK8_SUCCESS;
}