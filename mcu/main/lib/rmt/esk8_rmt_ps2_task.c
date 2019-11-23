#include <esk8_config.h>
#include <esk8_log.h>
#include <esk8_ps2.h>
#include <esk8_rmt.h>
#include "esk8_rmt_priv.h"

#include <unistd.h>


void
esk8_rmt_task_ps2(
    void* param
)
{
    esk8_err_t err;
    bool ps2_available = false;

    while (1)
    {
        err = esk8_ps2_mvmt_sync(esk8_rmt.hndl_ps2);
        if (err)
        {
            esk8_log_E(ESK8_TAG_RMT,
                "Could not start data stream. Err: %s\n",
                esk8_err_to_str(err)
            );

            /**
             * We might not have a ps2 device attatched.
             * If this is the case, we hold a second before
             * retrying.
             */
            if  (!ps2_available)
                sleep(1);
            else
                usleep(10 * 1000); // 10 ms

            continue;
        }

        ps2_available = true;
        esk8_log_I(ESK8_TAG_RMT,
            "OK on ps2 data stream.\n"
        );

        while(1)
        {
            esk8_ps2_mvmt_t mvmt;
            err = esk8_ps2_await_mvmt(
                esk8_rmt.hndl_ps2,
                &mvmt
            );

            if (err)
            {
                esk8_log_W(ESK8_TAG_RMT,
                    "Error awaiting ps2 mvmt packet: %s\n",
                    esk8_err_to_str(err)
                );

                break;
            }

            esk8_log_D(ESK8_TAG_RMT,
                "PS2 says x: %03d, y: %03d, btn: %s\n",
                mvmt.x,
                mvmt.y,
                mvmt.lft_btn?"Yes":"No"
            );

            err = esk8_rmt_incr_speed(mvmt.x);

            if (err)
                esk8_log_E(ESK8_TAG_RMT,
                    "Got err: %d (%s) updating speed\n",
                    err, esk8_err_to_str(err)
                );
        }
    }
}