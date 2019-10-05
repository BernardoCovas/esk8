#include <esk8_err.h>
#include <esk8_config.h>
#include <esk8_ps2.h>
#include <esk8_ps2_priv.h>

#include <driver/gpio.h>

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
    esk8_ps2_mvmt_frame_t* mvmt_frame = &ps2_hndl->mvmt_frame;

    int* idx = &frame->idx;

    if (*idx > 10 || *idx < 0)
        esk8_ps2_reset_frame(frame);

    switch(ps2_hndl->ps2_state)
    {
    case ESK8_PS2_STATE_NONE:
        break;

    case ESK8_PS2_STATE_SEND:
    {
        int bit = 0;

        switch (frame->idx)
        {
        case 0:  bit = 0; break;
        case 9:  bit = esk8_ps2_get_parity(frame->byte); break;
        case 10: bit = 1; break;
        default: bit = frame->byte & (1 << frame->idx); break;
        }

        gpio_set_level(
            ps2_cnfg->data_pin,
            bit
        );

        (*idx)++;

        if (*idx == 8)
        {
            esk8_ps2_reset_frame(frame);
            ps2_hndl->ps2_state = ESK8_PS2_STATE_RECV;
        }
    }
        break;

    case ESK8_PS2_STATE_RECV:
    case ESK8_PS2_STATE_MVMT:
    {
        int bit = gpio_get_level(ps2_hndl->ps2_cnfg.data_pin);

        esk8_err_t _err = ESK8_PS2_ERR_BAD_PCK;

        switch (frame->idx)
        {
        case 0:  if (bit) frame->err = _err; break;
        case 9:  if (esk8_ps2_get_parity(frame->byte) != bit) frame->err = _err; break;
        case 10: if (!bit) frame->err = _err; break;

        default:
            esk8_ps2_set_bit(
                &frame->byte,
                (*idx) - 1,
                bit
            );
            break;
        }

        ++frame->idx;
        if (frame->idx != 11)
            break;

        if (ESK8_PS2_STATE_RECV)
        {
            xQueueSendFromISR(
                ps2_hndl->rx_cmd_queue,
                &frame->byte,
                NULL
            );

            /**
             * Now that the response was received,
             * we can allow whoever wants to send something
             * to take the smphr.
             * TODO (b.covas): conext switch
             */
            xSemaphoreGiveFromISR(
                ps2_hndl->rx_tx_lock,
                NULL
            );

            break;
        }

        if (frame->err)
            mvmt_frame->err = frame->err;

        if  (
                !mvmt_frame->err &&
                 mvmt_frame->idx > 2
            )
        {
            mvmt_frame->err = ESK8_PS2_ERR_BAD_MVMT;
            break;
        }

        mvmt_frame->mvmt[mvmt_frame->idx] = frame->byte;
        mvmt_frame->idx++;

        if (mvmt_frame->idx == 3)
        {
            esk8_ps2_mvmt_t mvmt;
            mvmt.err = mvmt_frame->err;

            if (!mvmt.err)
            {
                bool sX, sY;
                sX = mvmt_frame->mvmt[0] & (1 << 4);
                sY = mvmt_frame->mvmt[0] & (1 << 5);

                mvmt.lft_btn   = mvmt_frame->mvmt[0] & 1;
                mvmt.x         = sX * -256 + mvmt_frame->mvmt[1];
                mvmt.y         = sY * -256 + mvmt_frame->mvmt[2];
            }

            xQueueSendFromISR(
                ps2_hndl->rx_mvt_queue,
                &mvmt,
                NULL
            );

            break;
        }

        break;
    }
    }
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

    ps2_hndl_def->rx_cmd_queue = xQueueCreate(
        ps2_config->rx_queue_len,
        sizeof(esk8_ps2_frame_t)
    );

    ps2_hndl_def->rx_mvt_queue = xQueueCreate(
        ps2_config->rx_queue_len,
        sizeof(esk8_ps2_mvmt_t)
    );

    ps2_hndl_def->rx_tx_lock = xSemaphoreCreateBinary();

    if  (
            !ps2_hndl_def->rx_cmd_queue ||
            !ps2_hndl_def->rx_mvt_queue ||
            !ps2_hndl_def->rx_tx_lock
        )
    {
        esk8_ps2_deinit(*ps2_hndl);
        return ESK8_ERR_OOM;
    }

    ps2_hndl_def->ps2_cnfg = *ps2_config;

    return ESK8_OK;
}


esk8_err_t
esk8_ps2_deinit(
    esk8_ps2_hndl_t ps2_hndl
)
{
    esk8_ps2_hndl_def_t* ps2_hndl_def = (esk8_ps2_hndl_def_t*)ps2_hndl;
    if (ps2_hndl_def->rx_cmd_queue)
        vQueueDelete(ps2_hndl_def->rx_cmd_queue);

    if (ps2_hndl_def->rx_mvt_queue)
        vQueueDelete(ps2_hndl_def->rx_mvt_queue);

    if (ps2_hndl_def->rx_tx_lock)
        vSemaphoreDelete(ps2_hndl_def->rx_tx_lock);

    free(ps2_hndl);

    return ESK8_OK;
}
