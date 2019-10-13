#include <esk8_err.h>
#include <esk8_log.h>
#include <esk8_pwm.h>
#include <esk8_ps2.h>
#include <esk8_config.h>
#include <esk8_remote.h>
#include <esk8_remote_priv.h>

#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gattc_api.h>
#include <esp_gap_ble_api.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifndef ESK8_ONBOARD_MAC_ADDR
#error "ESK8_ONBOARD_MAC_ADDR not defined. Set it to the board's mac address"
#endif


esk8_remote_t esk8_remote = { 0 };

void
esk8_remote_gattc_cb(
    esp_gattc_cb_event_t      event,
    esp_gatt_if_t             gattc_if,
    esp_ble_gattc_cb_param_t* param
);

void
esk8_remote_gap_cb(
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param
);

esk8_err_t
esk8_remote_start()
{
    if (esk8_remote.state)
        return ESK8_ERR_REMT_REINIT;

    esk8_remote = (esk8_remote_t) { 0 };
    esk8_remote.state = ESK8_REMOTE_STATE_INIT;

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
        return ESK8_NVS_NOT_AVAILABLE;

    esp_bt_controller_config_t bt_cnfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    esp_bt_controller_init(&bt_cnfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    esp_bluedroid_init();
    esp_bluedroid_enable();
    esp_ble_gap_register_callback(esk8_remote_gap_cb);
    esp_ble_gattc_register_callback(esk8_remote_gattc_cb);

    esk8_err_t err = esk8_pwm_sgnl_init_from_config_h(
        &esk8_remote.hndl_pwm
    );

    if (err)
    {
        esk8_remote_stop();
        return err;
    }

    err = esk8_ps2_init_from_config_h(
        &esk8_remote.hndl_ps2
    );

    if (err)
    {
        esk8_remote_stop();
        return err;
    }

    BaseType_t ps2_tsk = xTaskCreate(
        esk8_remote_task_ps2,
        "esk8_remote_task_ps2",
        2048, NULL, 1,
        &esk8_remote.task_ps2
    );

    if (ps2_tsk != pdPASS)
    {
        esk8_remote_stop();
        return ESK8_ERR_OOM;
    }

    err = esk8_btn_init_from_config_h(
        &esk8_remote.hndl_btn
    );

    if (err)
    {
        esk8_remote_stop();
        return err;
    }

    BaseType_t btn_tsk = xTaskCreate(
        esk8_remote_task_btn,
        "esk8_remote_task_btn",
        2048, NULL, 1,
        &esk8_remote.task_btn
    );

    if (btn_tsk != pdPASS)
    {
        esk8_remote_stop();
        return ESK8_ERR_OOM;
    }

    esk8_remote.state = ESK8_REMOTE_STATE_NOT_CONNECTED;

    return ESK8_OK;
}

esk8_err_t
esk8_remote_stop()
{
    if (esk8_remote.task_ble)
        vTaskDelete(esk8_remote.task_ble);

    if (esk8_remote.task_btn)
        vTaskDelete(esk8_remote.task_btn);

    if (esk8_remote.task_ps2)
        vTaskDelete(esk8_remote.task_ps2);

    if (esk8_remote.hndl_btn)
        esk8_btn_deinit(esk8_remote.hndl_btn);

    if (esk8_remote.hndl_ps2)
        esk8_ps2_deinit(esk8_remote.hndl_ps2);

    if (esk8_remote.hndl_pwm)
        esk8_pwm_sgnl_stop(esk8_remote.hndl_pwm);

    return ESK8_OK;
}

esk8_err_t
esk8_remote_incr_speed(
    int incr
)
{
    esk8_remote.speed += incr;
    esk8_remote.speed = esk8_remote.speed > 255 ? 255 : esk8_remote.speed;
    esk8_remote.speed = esk8_remote.speed < 0   ? 0   : esk8_remote.speed;

    esk8_log_I(ESK8_TAG_RMT,
        "Speed incr: %d. Now: %d\n",
        incr, esk8_remote.speed
    );

    esk8_err_t err = esk8_pwm_sgnl_set(
        esk8_remote.hndl_pwm,
        esk8_remote.speed
    );

    return err;
}

void
esk8_remote_gap_cb(
    esp_gap_ble_cb_event_t  event,
    esp_ble_gap_cb_param_t *param
)
{
    printf("[ GAP ] Got event: %d\n", event);

    switch (event)
    {
    case ESP_GAP_BLE_SCAN_RESULT_EVT:
    {
        if (esk8_remote.state != ESK8_REMOTE_STATE_SEARCHING)
        {
            esp_ble_gap_stop_scanning();
            break;
        }

        if (param->scan_rst.search_evt != ESP_GAP_SEARCH_INQ_RES_EVT)
            break;

        printf("[ GAP ] Dev: ");
        for (int i = 0; i < 6; i++)
            printf("%s%02x", i==0 ? "":":", param->scan_rst.bda[i]);

        printf(", RSSI: %d, evt: %d\n",
            param->scan_rst.rssi,
            param->scan_rst.search_evt
        );

        break;
    }

    default:
        break;
    }
}

void
esk8_remote_gattc_cb(
    esp_gattc_cb_event_t        event,
    esp_gatt_if_t               gattc_if,
    esp_ble_gattc_cb_param_t*   param
)
{
    switch (event)
    {
    case ESP_GATTC_DISCONNECT_EVT:
        break;

    case ESP_GATTC_CONNECT_EVT:
        break;

    default:
        break;
    }
}

esk8_err_t
esk8_remote_connect(
    uint32_t sec
)
{
    if (esk8_remote.state != ESK8_REMOTE_STATE_NOT_CONNECTED)
        return ESK8_ERR_REMT_BAD_STATE;

    return ESK8_OK;
}