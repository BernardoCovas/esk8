#include <esk8_log.h>
#include <esk8_err.h>
#include <esk8_config.h>
#include <esk8_ps2.h>
#include <esk8_ps2_priv.h>

#include <driver/gpio.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#define RX_QUEUE_LEN 1024


void
esk8_ps2_isr(
    void* param
)
{
    esk8_ps2_hndl_def_t* ps2_hndl = (esk8_ps2_hndl_def_t*)param;
    esk8_ps2_cnfg_t* ps2_cnfg = &ps2_hndl->ps2_cnfg;
    esk8_ps2_frame_t* frame = &ps2_hndl->inflight;

    switch(ps2_hndl->ps2_state)
    {
    case ESK8_PS2_STATE_RECV:
    case ESK8_PS2_STATE_MVMT:
    {
        int bit = gpio_get_level(ps2_cnfg->data_pin);
        switch (frame->idx)
        {
        case 0:
        case 9:
        case 10:
        case 11: // Ack bit
            break;
        default:
            frame->byte |= bit << (frame->idx - 1);
            break;
        }

        break;
    }
    case ESK8_PS2_STATE_SEND:
    {
        int bit = 0;
        switch (frame->idx)
        {
        case 0 : bit = 0; break;
        case 9 : bit = esk8_ps2_get_parity(frame->byte); break;
        case 10: bit = 1; break;
        default:
            bit = frame->byte & (1 << (frame->idx - 1));
            break;
        }

        gpio_set_level(ps2_cnfg->data_pin, bit);
        break;
    }
    default:
        break;
    }

    frame->idx++;
    if (frame->idx < 11)
        return;

    switch (ps2_hndl->ps2_state)
    {
    case ESK8_PS2_STATE_RECV:
        xQueueSendFromISR(ps2_hndl->rx_queue, frame, NULL);
        ps2_hndl->ps2_state = ESK8_PS2_STATE_MVMT;
        break;
    case ESK8_PS2_STATE_MVMT:
        xQueueSendFromISR(ps2_hndl->mv_queue, frame, NULL);
        break;
    case ESK8_PS2_STATE_SEND:
        gpio_set_direction(ps2_cnfg->data_pin, GPIO_MODE_INPUT);
        gpio_set_intr_type(ps2_cnfg->clock_pin, GPIO_INTR_NEGEDGE);
        if (frame->idx != 12)
            return;
        ps2_hndl->ps2_state = ESK8_PS2_STATE_RECV;
        break;

    default:
        break;
    }

    esk8_ps2_reset_frame(frame);
}


esk8_err_t
esk8_ps2_init(
    esk8_ps2_hndl_t* ps2_hndl,
    esk8_ps2_cnfg_t* ps2_config
)
{
    *ps2_hndl = calloc(1, sizeof(esk8_ps2_hndl_def_t));
    if (!ps2_hndl)
        return ESK8_ERR_OOM;

    esk8_ps2_hndl_def_t* ps2_hndl_def = (esk8_ps2_hndl_def_t*)*ps2_hndl;

    printf(I ESK8_TAG_PS2
        "PS2 Init with queue len: %d, timeout: %d ms, clock pin: %d, data pin: %d\n",
        ps2_config->rx_queue_len,
        ps2_config->rx_timeout_ms,
        ps2_config->clock_pin,
        ps2_config->data_pin
    );

    ps2_hndl_def->rx_queue = xQueueCreate(
        ps2_config->rx_queue_len,
        sizeof(esk8_ps2_frame_t)
    );

    ps2_hndl_def->mv_queue = xQueueCreate(
        ps2_config->rx_queue_len,
        sizeof(esk8_ps2_frame_t)
    );

    ps2_hndl_def->tx_lock = xSemaphoreCreateBinary();

    if  (
            !ps2_hndl_def->rx_queue ||
            !ps2_hndl_def->mv_queue ||
            !ps2_hndl_def->tx_lock
        )
    {
        esk8_ps2_deinit(*ps2_hndl);
        return ESK8_ERR_OOM;
    }

    ps2_hndl_def->ps2_cnfg = *ps2_config;

    gpio_num_t c_pin = ps2_config->clock_pin;
    gpio_num_t d_pin = ps2_config->data_pin;

    gpio_set_pull_mode(c_pin, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(d_pin, GPIO_PULLUP_ONLY);
    gpio_set_drive_capability(c_pin, GPIO_DRIVE_CAP_0);
    gpio_set_drive_capability(d_pin, GPIO_DRIVE_CAP_0);
    gpio_set_direction(c_pin, GPIO_MODE_INPUT);
    gpio_set_direction(d_pin, GPIO_MODE_INPUT);
    gpio_set_intr_type(c_pin, GPIO_INTR_NEGEDGE);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(c_pin, esk8_ps2_isr, ps2_hndl_def);

    /** Now we can allow bytes to be sent */
    xSemaphoreGive(ps2_hndl_def->tx_lock);

    return ESK8_OK;
}


esk8_err_t
esk8_ps2_deinit(
    esk8_ps2_hndl_t ps2_hndl
)
{
    esk8_ps2_hndl_def_t* ps2_hndl_def = (esk8_ps2_hndl_def_t*)ps2_hndl;
    if (ps2_hndl_def->rx_queue)
        vQueueDelete(ps2_hndl_def->rx_queue);

    if (ps2_hndl_def->mv_queue)
        vQueueDelete(ps2_hndl_def->mv_queue);

    if (ps2_hndl_def->tx_lock)
        vSemaphoreDelete(ps2_hndl_def->tx_lock);

    free(ps2_hndl);

    return ESK8_OK;
}
