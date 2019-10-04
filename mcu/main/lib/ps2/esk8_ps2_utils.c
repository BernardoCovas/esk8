#include <esk8_err.h>
#include <esk8_log.h>
#include "esk8_ps2_utils.h"

#include <driver/gpio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include <memory.h>


void
esk8_ps2_set_bit(
    uint8_t* byte,
    int      idx,
    int      val
)
{
    if (idx > 7)
        return;

    val = val ? 1 : 0;

    if (val)
        *byte |= (1 << idx);
    else
        *byte &= ~(1 << idx);
}


void
esk8_ps2_reset_frame(
    esk8_ps2_frame_t* frame
)
{
    memset(frame, 0, sizeof(esk8_ps2_frame_t));
}


int
esk8_ps2_get_parity(
    uint8_t x
)
{
    x = (x & 0x0F)^(x >> 4);
    x = (x & 0x03)^(x >> 2);
    x = (x & 0x01)^(x >> 1);

    return !(x & 1);
}


void
esk8_ps2_send_byte(
    esk8_ps2_hndl_t hndl,
    uint8_t         byte
)
{
    esk8_ps2_hndl_def_t* ps2_hndl = (esk8_ps2_hndl_def_t*)hndl;

    int c_pin = ps2_hndl->ps2_cnfg.clock_pin;
    int d_pin = ps2_hndl->ps2_cnfg.data_pin;

    gpio_set_direction(c_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(d_pin, GPIO_MODE_OUTPUT);

    gpio_set_level(c_pin, 0);
    vTaskDelay(60 / portTICK_PERIOD_MS / 1000);

    gpio_set_intr_type(c_pin, GPIO_INTR_POSEDGE);
    gpio_set_direction(c_pin, GPIO_MODE_INPUT);
}

esk8_err_t
esk8_ps2_send_cmd(
    esk8_ps2_hndl_t hndl,
    esk8_ps2_cmd_t    cmd
)
{
    esk8_err_t err;

    esk8_ps2_hndl_def_t* ps2_hndl = (esk8_ps2_hndl_def_t*)hndl;

    if  (
            xSemaphoreTake(
                ps2_hndl->rx_tx_lock,
                ps2_hndl->ps2_cnfg.xr_timeout_ms / portTICK_PERIOD_MS
            ) != pdTRUE
        )
    {
        return ESK8_PS2_ERR_TIMEOUT;
    }

    esk8_ps2_send_byte(hndl, (uint8_t)cmd);
    ps2_hndl->ps2_state = ESK8_PS2_STATE_SEND;
 
    uint8_t resp = 0;
    err = esk8_ps2_await_rsp(
        ps2_hndl,
        &resp
    );

    if (err)
        return err;

    printf(I ESK8_TAG_PS2
        "Got response: %d for cmd: %d\n",
        resp, ps2Cmd
    );

    if (resp == ESK8_PS2_RES_RESEND)
        return ESK8_PS2_ERR_RESEND;

    if (resp != ESK8_PS2_RES_ACK)
        return ESK8_PS2_ERR_NO_ACK;

    return ESK8_OK;
}


esk8_err_t
esk8_ps2_await_rsp(
    esk8_ps2_hndl_t hndl,
    uint8_t*        out_byte
)
{
    esk8_ps2_hndl_def_t* ps2_hndl = (esk8_ps2_hndl_def_t*)hndl;
    esk8_ps2_cnfg_t* ps2_cnfg = ps2_hndl->ps2_cnfg;

    ps2_hndl->ps2_state = ESK8_PS2_STATE_RECV;
    esk8_ps2_frame_t frame;
    if  (
            xQueueReceive(
                ps2_hndl->rx_cmd_queue,
                &frame,
                ps2_cnfg->xr_timeout_ms
            ) != pdPASSS
        )
    {
        return ESK8_PS2_ERR_BYTE_TIMEOUT;
    }

    if (frame.err)
        return frame.err;

    (*out_byte) = frame.byte;
    return ESK8_OK;
}