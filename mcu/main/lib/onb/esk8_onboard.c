#include <esk8_config.h>
#include <esk8_log.h>
#include <esk8_bms.h>
#include <esk8_btn.h>
#include <esk8_pwm.h>
#include <esk8_onboard.h>
#include <esk8_bles/esk8_bles_app_status.h>

#include "esk8_onboard_priv.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


esk8_onboard_t esk8_onboard = { 0 };

esk8_err_t
esk8_onboard_start(
    esk8_onboard_cnfg_t* cnfg
)
{
    if (esk8_onboard.state)
        return ESK8_ERR_BLE_APPS_INIT_REINIT;

    esk8_onboard.state = ESK8_RIDE_STATE_INIT;
    esk8_onboard.cnfg  = (*cnfg);

    esk8_onboard.bms_stat = calloc(
        ESK8_UART_BMS_CONF_NUM, sizeof(esk8_bms_status_t));

    if (!esk8_onboard.bms_stat)
        return ESK8_ERR_OOM;

    esk8_onboard.bms_deep_stat = calloc(
        ESK8_UART_BMS_CONF_NUM, sizeof(esk8_bms_deep_status_t));

    if (!esk8_onboard.bms_deep_stat)
    {
        esk8_onboard_stop();
        return ESK8_ERR_OOM;
    }

    esk8_err_t err = esk8_bms_init_from_config_h(
        &esk8_onboard.hndl_bms
    );

    if (err)
    {
        esk8_onboard_stop();
        return err;
    }

    err = esk8_pwm_sgnl_init_from_config_h(
        &esk8_onboard.hndl_pwm
    );

    if (err)
    {
        esk8_onboard_stop();
        return err;
    }

    if  (
            xTaskCreate(
                esk8_onboard_task_bms,
                "ESK8_TASK_BMS", 2048,
                cnfg, 1, &esk8_onboard.task_bms
            ) != pdPASS)
    {
        esk8_onboard_stop();
        return ESK8_ERR_OOM;
    }

    err = esk8_btn_init_from_config_h(
        &esk8_onboard.hndl_btn
    );

    if (err)
    {
        esk8_onboard_stop();
        return err;
    }

    if  (
            xTaskCreate(
                esk8_onboard_task_btn,
                "ESK8_TASK_BTN", 2048,
                cnfg, 1, &esk8_onboard.task_btn
            ) != pdPASS
        )
    {
        esk8_onboard_stop();
        return ESK8_ERR_OOM;
    }

    esk8_onboard.state = ESK8_RIDE_STATE_RUNNING;
    return ESK8_OK;
}

esk8_err_t
esk8_onboard_set_speed(
    uint8_t speed
)
{
    if (!esk8_onboard.state)
        return ESK8_ERR_OBRD_NOINIT;

    esk8_onboard.now_speed = speed;

    esk8_onboard.err = esk8_pwm_sgnl_set(
        esk8_onboard.hndl_pwm, speed
    );

    esk8_log_I(ESK8_TAG_ONB, "Got: '%s' on pwm update.\n",
        esk8_err_to_str(esk8_onboard.err)
    );

    esk8_bles_app_status_speed(speed);

    return esk8_onboard.err;
}

esk8_err_t
esk8_onboard_stop(
)
{
    if (esk8_onboard.task_btn)
        vTaskDelete(esk8_onboard.task_btn);

    if (esk8_onboard.task_btn)
        vTaskDelete(esk8_onboard.task_btn);

    if (esk8_onboard.bms_stat)
        free(esk8_onboard.bms_stat);

    if (esk8_onboard.bms_deep_stat)
        free(esk8_onboard.bms_deep_stat);

    esk8_onboard = (esk8_onboard_t){ 0 };
    return ESK8_OK;
}
