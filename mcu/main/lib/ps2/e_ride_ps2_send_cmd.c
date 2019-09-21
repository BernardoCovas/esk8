#include <esk8_log.h>
#include <esk8_err.h>
#include <esk8_ps2.h>

#include <stdint.h>


esk8_err_t esk8_ps2_send_cmd(

    esk8_ps2_handle_t* ps2Handle,
    esk8_ps2_cmd_t     ps2Cmd,
    int                timeOut_ms

)
{
    esk8_err_t err;

    err = esk8_ps2_send_byte(ps2Handle, (uint8_t)ps2Cmd);
    if (err)
        return err;

    uint8_t resp = 0;
    err = esk8_ps2_await_byte(ps2Handle, &resp, timeOut_ms);
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

    return ESK8_SUCCESS;
}