#include <e_ride_err.h>
#include <e_ride_ps2.h>

#include <stdint.h>


e_ride_err_t e_ride_ps2_send_cmd(

    e_ride_ps2_handle_t* ps2Handle,
    e_ride_ps2_cmd_t     ps2Cmd,
    int                  timeOut_ms

)
{
    e_ride_err_t errCode;

    errCode = e_ride_ps2_send_byte(ps2Handle, (uint8_t)ps2Cmd);
    if (errCode)
        return errCode;

    uint8_t respByte;
    errCode = e_ride_ps2_await_byte(ps2Handle, &respByte, timeOut_ms);
    if (errCode)
        return errCode;

    if (respByte != E_RIDE_PS2_RES_ACK)
        return E_RIDE_PS2_ERR_NO_ACK;

    return E_RIDE_SUCCESS;
}