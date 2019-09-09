#include <esk8_bms.h>
#include <esk8_btn.h>
#include <esk8_log.h>

#include <esk8_onboard.h>
#include <ble_apps/esk8_ble_app_status.h>

void
esk8_onboard_task_btn(
    void* param
)
{
    if (!esk8_onboard.state)
        return;

    esk8_btn_cnfg_t* btn_cnfg = &esk8_onboard.btn_cnfg;

    esk8_err_t err;
    esk8_btn_press_t btn_press;

    while(1)
    {
        err = esk8_btn_await_press(btn_cnfg, &btn_press, esk8_onboard.cnfg.btn_timeout_ms);
        if (err)
            printf(ESK8_TAG_BTN "Got: '%s' \n", esk8_err_to_str(err));
        else
            printf(ESK8_TAG_BTN "Got press: %d \n", btn_press);
    }
}
