#include <esk8_onboard.h>
#include <ble_apps/esk8_ble_app_status.h>

#include <esk8_config.h>
#include <esk8_log.h>
#include <esk8_bms.h>
#include <esk8_btn.h>
#include <esk8_pwm.h>


esk8_onboard_t esk8_onboard = { 0 };

esk8_err_t
esk8_onboard_start(
    esk8_onboard_cnfg_t* cnfg
)
{
    if (esk8_onboard.state)
        return ESK8_BLE_INIT_REINIT;

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

    esk8_err_t err = esk8_bms_init_from_config_h(&esk8_onboard.bms_cnfg);
    if (err)
    {
        esk8_onboard_stop();
        return err;
    }

    err = esk8_pwm_sgnl_init(&esk8_onboard.pwm_cnfg);
    if (err)
    {
        esk8_onboard_stop();
        return err;
    }

    if (xTaskCreate(
        esk8_onboard_task_bms,
        "ESK8_TASK_BMS", 2048, cnfg, 1, &esk8_onboard.bms_stat_task) != pdPASS)
    {
        esk8_onboard_stop();
        return ESK8_ERR_OOM;
    }

    esk8_onboard.btn_cnfg = (esk8_btn_cnfg_t) {
        .btn_debounce_ms  = 10,
        .btn_gpio         = GPIO_NUM_0,
        .btn_longPress_ms = 2000,
        .btn_tmrGrp       = 0,
        .btn_tmrIdx       = 1
    };

    err = esk8_btn_init(&esk8_onboard.btn_cnfg);
    if (err)
    {
        esk8_onboard_stop();
        return err;
    }

    if (xTaskCreate(
        esk8_onboard_task_btn,
        "ESK8_TASK_BTN", 2048, cnfg, 1, &esk8_onboard.btn_stat_task) != pdPASS)
    {
        esk8_onboard_stop();
        return ESK8_ERR_OOM;
    }

    esk8_onboard.state = ESK8_RIDE_STATE_RUNNING;
    return ESK8_SUCCESS;
}

esk8_err_t
esk8_onboard_set_speed(
    uint8_t speed
)
{
    if (!esk8_onboard.state)
        return ESK8_ERR_OBRD_NOINIT;

    esk8_onboard.speed = speed;

    esk8_onboard.err = esk8_pwm_sgnl_set(
        &esk8_onboard.pwm_cnfg, speed);

    printf(ESK8_TAG_ONB "Got: '%s' on pwm update.\n",
        esk8_err_to_str(esk8_onboard.err));

    esk8_ble_app_status_speed(speed);

    return esk8_onboard.err;
}

esk8_err_t
esk8_onboard_stop(
)
{
    if (esk8_onboard.bms_stat_task)
        vTaskDelete(esk8_onboard.bms_stat_task);

    if (esk8_onboard.btn_stat_task)
        vTaskDelete(esk8_onboard.btn_stat_task);

    if (esk8_onboard.bms_stat)
        free(esk8_onboard.bms_stat);

    if (esk8_onboard.bms_deep_stat)
        free(esk8_onboard.bms_deep_stat);

    esk8_onboard = (esk8_onboard_t){ 0 };
    return ESK8_SUCCESS;
}
