#include <esk8_config.h>
#include <esk8_ps2.h>
#include <esk8_err.h>


esk8_err_t esk8_ps2_await_mvmnt(

    esk8_ps2_handle_t* ps2Handle,
    esk8_ps2_mvmnt_t * outMvmnt

)
{
    int timeOut_ms = ESK8_PS2_MOVEMENT_TIMEOUT_MS;
    esk8_err_t errCode;
    uint8_t frame[3];

    for (int i=0; i<3; i++)
    {
        errCode = esk8_ps2_await_byte(ps2Handle, &frame[i], timeOut_ms);
        if(errCode != ESK8_SUCCESS)
            return errCode;

        timeOut_ms = ESK8_PS2_MOVEMENT_BYTE_TIMEOUT_MS;
    }

    bool sX, sY;
    sX = frame[0] & (1 << 4);
    sY = frame[0] & (1 << 5);

    outMvmnt->lftBtn    = frame[0] & 1;
    outMvmnt->x         = sX * -256 + frame[1];
    outMvmnt->y         = sY * -256 + frame[2];

    return ESK8_SUCCESS;
}