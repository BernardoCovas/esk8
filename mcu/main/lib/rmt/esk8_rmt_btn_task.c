#include <esk8_err.h>
#include <esk8_log.h>
#include <esk8_btn.h>
#include <esk8_rmt.h>

#include <esk8_blec_apps.h>

#include "esk8_rmt_priv.h"


void
esk8_rmt_task_btn(
    void* param
)
{
    esk8_err_t err;
    esk8_btn_press_t press;

    while(1)
    {
        err = esk8_btn_await_press(
            esk8_rmt.hndl_btn,
            &press
        );

        if (err)
        {
            esk8_log_E(ESK8_TAG_BTN,
                "Got: %s\n",
                esk8_err_to_str(err)
            );

            continue;
        }

        esk8_log_I(ESK8_TAG_BTN,
            "Got press: %s\n",
            press ? "ESK8_BTN_LONGPRESS":"ESK8_BTN_PRESS"
        );

        switch (press)
        {
        case ESK8_BTN_LONGPRESS:
            esk8_blec_search_start(~0);
            break;

        case ESK8_BTN_PRESS:
            esk8_blec_search_stop();
            esk8_blec_dscn();

            break;

        default:
            break;
        }
    }
}