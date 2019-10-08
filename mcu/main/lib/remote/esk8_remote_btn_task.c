#include <esk8_err.h>
#include <esk8_log.h>
#include <esk8_btn.h>
#include <esk8_remote.h>


void
esk8_remote_task_btn(
    void* param
)
{
    esk8_err_t err;
    esk8_btn_hndl_t hndl = (esk8_btn_hndl_t)param;
    esk8_btn_press_t press;

    while(1)
    {
        err = esk8_btn_await_press(hndl, &press);
        if (err)
            continue;

        printf(I ESK8_TAG_BTN
            "Got press: %s\n",
            press ? "ESK8_BTN_LONGPRESS":"ESK8_BTN_PRESS"
        );
    }
}