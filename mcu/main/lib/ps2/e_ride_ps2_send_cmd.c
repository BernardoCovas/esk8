#include <esk8_err.h>
#include <esk8_ps2.h>

#include <stdint.h>


esk8_err_t esk8_ps2_send_cmd(

    esk8_ps2_handle_t* ps2Handle,
    esk8_ps2_cmd_t     ps2Cmd,
    int                  timeOut_ms

)
{
    esk8_err_t errCode;

    errCode = esk8_ps2_send_byte(ps2Handle, (uint8_t)ps2Cmd);
    if (errCode)
        return errCode;

    uint8_t respByte;
    errCode = esk8_ps2_await_byte(ps2Handle, &respByte, timeOut_ms);
    if (errCode)
        return errCode;

    if (respByte != ESK8_PS2_RES_ACK)
        return ESK8_PS2_ERR_NO_ACK;

    return ESK8_SUCCESS;
}