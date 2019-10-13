#include <esk8_err.h>
#include <esk8_log.h>
#include <esk8_pwm.h>
#include <esk8_ps2.h>
#include <esk8_config.h>
#include <esk8_rmt.h>
#include <esk8_ble_appc.h>

#include "esk8_rmt_priv.h"

#include <nvs_flash.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


esk8_rmt_t esk8_rmt = { 0 };

esk8_err_t
esk8_rmt_start()
{
    if (esk8_rmt.state)
        return ESK8_ERR_REMT_REINIT;

    esk8_rmt = (esk8_rmt_t) { 0 };
    esk8_rmt.state = ESK8_REMOTE_STATE_INIT;

    esp_err_t ret = nvs_flash_init();

    if  (
            ret == ESP_ERR_NVS_NO_FREE_PAGES ||
            ret == ESP_ERR_NVS_NEW_VERSION_FOUND
        )
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    if (ret)
        return ESK8_ERR_NVS_NOT_AVAILABLE;

    esk8_err_t err = esk8_pwm_sgnl_init_from_config_h(
        &esk8_rmt.hndl_pwm
    );

    if (err)
    {
        esk8_rmt_stop();
        return err;
    }

    err = esk8_ps2_init_from_config_h(
        &esk8_rmt.hndl_ps2
    );

    if (err)
    {
        esk8_rmt_stop();
        return err;
    }

    BaseType_t ps2_tsk = xTaskCreate(
        esk8_rmt_task_ps2,
        "esk8_rmt_task_ps2",
        2048, NULL, 1,
        &esk8_rmt.task_ps2
    );

    if (ps2_tsk != pdPASS)
    {
        esk8_rmt_stop();
        return ESK8_ERR_OOM;
    }

    err = esk8_btn_init_from_config_h(
        &esk8_rmt.hndl_btn
    );

    if (err)
    {
        esk8_rmt_stop();
        return err;
    }

    BaseType_t btn_tsk = xTaskCreate(
        esk8_rmt_task_btn,
        "esk8_rmt_task_btn",
        2048, NULL, 1,
        &esk8_rmt.task_btn
    );

    if (btn_tsk != pdPASS)
    {
        esk8_rmt_stop();
        return ESK8_ERR_OOM;
    }

    esk8_rmt.state = ESK8_REMOTE_STATE_NOT_CONNECTED;

    return ESK8_OK;
}

esk8_err_t
esk8_rmt_stop()
{
    if (esk8_rmt.task_ble)
        vTaskDelete(esk8_rmt.task_ble);

    if (esk8_rmt.task_btn)
        vTaskDelete(esk8_rmt.task_btn);

    if (esk8_rmt.task_ps2)
        vTaskDelete(esk8_rmt.task_ps2);

    if (esk8_rmt.hndl_btn)
        esk8_btn_deinit(esk8_rmt.hndl_btn);

    if (esk8_rmt.hndl_ps2)
        esk8_ps2_deinit(esk8_rmt.hndl_ps2);

    if (esk8_rmt.hndl_pwm)
        esk8_pwm_sgnl_stop(esk8_rmt.hndl_pwm);

    return ESK8_OK;
}

esk8_err_t
esk8_rmt_incr_speed(
    int incr
)
{
    esk8_rmt.speed += incr;
    esk8_rmt.speed = esk8_rmt.speed > 255 ? 255 : esk8_rmt.speed;
    esk8_rmt.speed = esk8_rmt.speed < 0   ? 0   : esk8_rmt.speed;

    esk8_log_I(ESK8_TAG_RMT,
        "Speed incr: %d. Now: %d\n",
        incr, esk8_rmt.speed
    );

    esk8_err_t err = esk8_pwm_sgnl_set(
        esk8_rmt.hndl_pwm,
        esk8_rmt.speed
    );

    return err;
}


esk8_err_t
esk8_rmt_connect(
    uint32_t sec
)
{
    if (esk8_rmt.state != ESK8_REMOTE_STATE_NOT_CONNECTED)
        return ESK8_ERR_REMT_BAD_STATE;

    return ESK8_OK;
}