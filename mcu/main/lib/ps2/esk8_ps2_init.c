#include <esk8_err.h>
#include <esk8_config.h>
#include <esk8_ps2_v2.h>
#include <esk8_ps2_v2_priv.h>

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
    esk8_ps2_frame_t* frame = &ps2_hndl->inflight;
    int* idx = &frame->idx;

    switch(ps2_hndl->ps2_state)
    {
        case ESK8_PS2_STATE_NONE:
            break;

        case ESK8_PS2_STATE_SEND:
        {
            
        }
            break;

        case ESK8_PS2_STATE_RECV:
        case ESK8_PS2_STATE_MVMT:
        {

        }
            break;
    }
}


esk8_err_t
esk8_ps2_init(
    esk8_ps2_hndl_t* ps2_hndl,
    esk8_ps2_config_t* ps2_config
)
{
    *ps2_hndl = calloc(1, sizeof(esk8_ps2_hndl_def_t));
    if (!ps2_hndl)
        return ESK8_ERR_OOM;

    esk8_ps2_hndl_def_t* ps2_hndl_def = (esk8_ps2_hndl_def_t*)*ps2_hndl;

    ps2_hndl_def->rx_cmd_queue = xQueueCreate(
        ps2_config->rx_queue_len, 1
    );

    ps2_hndl_def->rx_mvt_queue = xQueueCreate(
        ps2_config->rx_queue_len, 1
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

    return ESK8_SUCCESS;
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

    return ESK8_SUCCESS;
}
