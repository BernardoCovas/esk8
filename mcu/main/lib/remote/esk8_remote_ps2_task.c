#include <esk8_log.h>
#include <esk8_config.h>
#include <esk8_remote.h>
#include <esk8_ps2.h>

#include <unistd.h>


void
esk8_remote_task_ps2(
    void* param
)
{
    esk8_err_t err;
    esk8_ps2_hndl_t ps2_hndl;
    bool ps2_available = false;

_start:
    err = esk8_ps2_init_from_config_h(&ps2_hndl);
    if (err)
    {
        printf(E ESK8_TAG_RMT
            "Could not init ps2. Err: %s\n",
            esk8_err_to_str(err)
        );

        sleep(1);
        goto _start;
    }

    while (1)
    {
        err = esk8_ps2_mvmt_sync(ps2_hndl);

        if (err)
        {
            printf(E ESK8_TAG_RMT
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
                usleep(20 * 1000); // 10 ms

            continue;
        }

        ps2_available = true;
        printf(I ESK8_TAG_RMT
            "OK on ps2 data stream.\n"
        );

        while(1)
        {
            esk8_ps2_mvmt_t mvmt;
            err = esk8_ps2_await_mvmt(
                ps2_hndl,
                &mvmt
            );

            if (err)
            {
                printf(W ESK8_TAG_RMT
                    "Error awaiting ps2 mvmt packet: %s\n",
                    esk8_err_to_str(err)
                );

                break;
            }

            printf(I ESK8_TAG_RMT
                "PS2 says x: %03d, y: %03d, btn: %s\n",
                mvmt.x,
                mvmt.y,
                mvmt.lft_btn?"Yes":"No"
            );

            esk8_remote_incr_speed(mvmt.x);
        }
    }

}