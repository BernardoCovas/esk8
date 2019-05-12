#include <e_skate_config.h>
#include <e_skate_ps2.h>
#include <e_skate_err.h>


e_skate_err_t e_skate_ps2_await_mvmnt(

    e_skate_ps2_handle_t* ps2Handle,
    e_skate_ps2_mvmnt_t * outMvmnt

)
{
    int timeOut_ms = E_SKATE_PS2_MOVEMENT_TIMEOUT_MS;
    e_skate_err_t errCode;
    uint8_t frame[3];

    for (int i=0; i<3; i++)
    {
        errCode = e_skate_ps2_await_byte(ps2Handle, &frame[i], timeOut_ms);
        if(errCode != E_SKATE_SUCCESS)
            return errCode;

        timeOut_ms = 50;
    }

    bool sX, sY;

    sX = frame[0] & (1 << 4);
    sY = frame[0] & (1 << 5);

    outMvmnt->lftBtn    = frame[0] & 1;
    outMvmnt->x         = sX * -256 + frame[1];
    outMvmnt->y         = sY * -256 + frame[2];

    return E_SKATE_SUCCESS;
}