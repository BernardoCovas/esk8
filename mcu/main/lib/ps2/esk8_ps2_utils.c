#include <esk8_err.h>
#include <esk8_log.h>
#include <esk8_ps2.h>
#include <esk8_ps2_priv.h>

#include <driver/gpio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include <memory.h>

#define PCK_CMD_WAIT_ms 20 // ms to wait between cmds

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
    esk8_ps2_reset_frame(&ps2_hndl->inflight); // Whatever byte was inflight is lost.
    ps2_hndl->inflight.byte = byte;
    ps2_hndl->ps2_state = ESK8_PS2_STATE_SEND;
    vTaskDelay(1 / portTICK_PERIOD_MS);

    gpio_set_direction(c_pin, GPIO_MODE_INPUT);
    gpio_set_intr_type(c_pin, GPIO_INTR_POSEDGE);
}


esk8_err_t
esk8_ps2_send_cmd(
    esk8_ps2_hndl_t hndl,
    esk8_ps2_cmd_t    cmd
)
{
    esk8_err_t err = ESK8_OK;
    esk8_ps2_hndl_def_t* ps2_hndl = (esk8_ps2_hndl_def_t*)hndl;

    if  (
            xSemaphoreTake(
                ps2_hndl->tx_lock,
                ps2_hndl->ps2_cnfg.rx_timeout_ms / portTICK_PERIOD_MS
            ) != pdTRUE
        )
    {
        return ESK8_PS2_ERR_TIMEOUT;
    }

    esk8_ps2_send_byte(hndl, (uint8_t)cmd);

    uint8_t resp = 0;
    err = esk8_ps2_await_rsp(
        ps2_hndl,
        &resp
    );

    if (err)
        goto cleanup;

    printf(I ESK8_TAG_PS2
        "Got response: 0x%02x for cmd: 0x%02x\n",
        resp, cmd
    );

    if (resp == ESK8_PS2_RES_RESEND)
    {
        err = ESK8_PS2_ERR_RESEND;
        goto cleanup;
    }

    if (resp == ESK8_PS2_RES_ERROR)
    {
        err = ESK8_PS2_ERR_ERROR;
        goto cleanup;
    }

    if (resp != ESK8_PS2_RES_ACK)
        err = ESK8_PS2_ERR_NO_ACK;

cleanup:
    esk8_ps2_reset_frame(&ps2_hndl->inflight);
    xSemaphoreGive(ps2_hndl->tx_lock);
    return err;
}


esk8_err_t
esk8_ps2_await_rsp(
    esk8_ps2_hndl_t hndl,
    uint8_t*        out_byte
)
{
    esk8_ps2_hndl_def_t* ps2_hndl = (esk8_ps2_hndl_def_t*)hndl;
    esk8_ps2_cnfg_t* ps2_cnfg = &ps2_hndl->ps2_cnfg;

    esk8_ps2_frame_t frame;

    if  (
            xQueueReceive(
                ps2_hndl->rx_queue,
                &frame,
                ps2_cnfg->rx_timeout_ms / portTICK_PERIOD_MS
            ) != pdTRUE
        )
    {
        return ESK8_PS2_ERR_RESP_TIMEOUT;
    }

    if (frame.err)
    {
        printf(E ESK8_TAG_PS2
            "Got err: %s in frame.\n",
            esk8_err_to_str(frame.err)
        );
        return frame.err;
    }

    (*out_byte) = frame.byte;
    return ESK8_OK;
}


esk8_err_t
esk8_ps2_mvmt_sync(
    esk8_ps2_hndl_t hndl
)
{
    esk8_err_t err = ESK8_OK;
    esk8_ps2_hndl_def_t* ps2_hndl = (esk8_ps2_hndl_def_t*)hndl;

    ESK8_ERRCHECK_THROW(
        esk8_ps2_send_cmd(hndl, ESK8_PS2_CMD_DATA_DISABLE)
    );

    xQueueReset(ps2_hndl->mv_queue);
    ps2_hndl->sqnc_frame.idx = 0;

    vTaskDelay(PCK_CMD_WAIT_ms / portTICK_PERIOD_MS);

    ESK8_ERRCHECK_THROW(
        esk8_ps2_send_cmd(hndl, ESK8_PS2_CMD_DATA_ENABLE)
    );

    xSemaphoreGive(ps2_hndl->tx_lock);
    return err;
}


esk8_err_t
esk8_ps2_await_mvmt(
    esk8_ps2_hndl_t hndl,
    esk8_ps2_mvmt_t* out_mvmt
)
{
    esk8_ps2_frame_t frame;
    esk8_ps2_hndl_def_t* ps2_hndl = (esk8_ps2_hndl_def_t*)hndl;
    esk8_ps2_cnfg_t* ps2_cnfg = &ps2_hndl->ps2_cnfg;
    esk8_ps2_sqnc_frame_t* sqnc = &ps2_hndl->sqnc_frame;

    while (sqnc->idx < 3)
    {
        BaseType_t recv = xQueueReceive(
            ps2_hndl->mv_queue,
            &frame,
            ps2_cnfg->rx_timeout_ms / portTICK_PERIOD_MS
        );

        if (recv != pdTRUE)
            return ESK8_PS2_ERR_TIMEOUT;

        if (frame.err)
        {
            printf(E ESK8_TAG_PS2
                "Gor error in packet: %s\n",
                esk8_err_to_str(frame.err)
            );

            return frame.err;
        }

        sqnc->mvmt[sqnc->idx] = frame.byte;
        sqnc->idx++;
    }

    if (!(sqnc->mvmt[0] & (1 << 3)))
        return ESK8_PS2_ERR_BAD_MVMT;

    bool sX, sY;
    sX = sqnc->mvmt[0] & (1 << 4);
    sY = sqnc->mvmt[0] & (1 << 5);

    out_mvmt->lft_btn = sqnc->mvmt[0] & 1;
    out_mvmt->x       = sX * -256 + sqnc->mvmt[1];
    out_mvmt->y       = sY * -256 + sqnc->mvmt[2];

    sqnc->idx = 0;
    return ESK8_OK;
}
