#include <esk8_bms.h>
#include <esk8_btn.h>
#include <esk8_log.h>
#include <esk8_onboard.h>
#include <esk8_ble_s/esk8_ble_app_status.h>

#include "esk8_onboard_priv.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


void
esk8_onboard_task_bms(
    void* param
)
{
    if (!esk8_onboard.state)
        return;

    esk8_err_t err;
    esk8_onboard_cnfg_t* cnfg = (esk8_onboard_cnfg_t*)param;

    while (1)
    {
        for (int i=0; i<ESK8_UART_BMS_CONF_NUM; i++)
        {

            esk8_bms_set_pin(esk8_onboard.hndl_bms, i);
            err = esk8_bms_get_status(
                esk8_onboard.hndl_bms,
                &esk8_onboard.bms_stat[i]
            );

            esk8_log_I(ESK8_TAG_ONB,
                "Got: %s reading BMS status at index: %d.\n",
                esk8_err_to_str(err), i
            );

            err = esk8_ble_app_status_bms_shallow(
                &esk8_onboard.bms_stat[i],
                err, i
            );

            esk8_log_I(ESK8_TAG_ONB,
                "Got: %s updating shallow BMS.\n",
                esk8_err_to_str(err)
            );

            err = esk8_bms_get_deep_status(
                esk8_onboard.hndl_bms,
                &esk8_onboard.bms_deep_stat[i]
            );

            esk8_log_I(ESK8_TAG_ONB,
                "Got: %s reading BMS deep status at index: %d.\n",
                esk8_err_to_str(err), i
            );

            err = esk8_ble_app_status_bms_deep(
                &esk8_onboard.bms_deep_stat[i],
                err, i
            );

            esk8_log_I(ESK8_TAG_ONB,
                "Got: %s updating deep BMS.\n",
                esk8_err_to_str(err)
            );

            /* We might wait quite a long time here */
            vTaskDelay(cnfg->bms_update_ms / portTICK_PERIOD_MS);
        }
    }
}
