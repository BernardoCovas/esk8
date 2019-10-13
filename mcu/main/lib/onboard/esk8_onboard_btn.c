#include <esk8_btn.h>
#include <esk8_log.h>

#include <esk8_onboard.h>
#include <esk8_onboard_priv.h>
#include <ble_apps/esk8_ble_app_status.h>

void
esk8_onboard_task_btn(
    void* param
)
{
    esk8_err_t err;
    esk8_btn_press_t press;

    while(1)
    {
        err = esk8_btn_await_press(
            esk8_onboard.hndl_btn,
            &press
        );

        if (err)
        {
            esk8_log_E(ESK8_TAG_BTN,
                "Got err: %s\n",
                esk8_err_to_str(err)
            );
        }
        else
        {
            esk8_log_I(ESK8_TAG_BTN,
                "Got press: %s\n",
                press ? "ESK8_BTN_LONGPRESS":"ESK8_BTN_PRESS"
            );
        }
        
    }
}
