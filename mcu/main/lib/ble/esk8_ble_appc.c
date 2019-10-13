#include <esk8_err.h>
#include <esk8_ble_appc.h>

#include "esk8_ble_appc_priv.h"

#include <esp_bt.h>
#include <nvs_flash.h>
#include <esp_bt_main.h>
#include <esp_gattc_api.h>
#include <esp_gap_ble_api.h>


static esk8_ble_appc_hndl_t esk8_ble_appc_hndl = { 0 };

esk8_err_t
esk8_ble_appc_init(
    uint n_apps_max
)
{
    if (esk8_ble_appc_hndl.app_ctx_l)
        return ESK8_ERR_BLE_APPC_INIT_REINIT;

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

    esp_bt_controller_config_t bt_cnfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    esp_bt_controller_init(&bt_cnfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    esp_bluedroid_init();
    esp_bluedroid_enable();
    esp_ble_gap_register_callback(esk8_ble_appc_gap_cb);
    esp_ble_gattc_register_callback(esk8_ble_appc_gattc_cb);

    esk8_ble_appc_hndl = (esk8_ble_appc_hndl_t) { 0 };

    esk8_ble_appc_hndl.app_ctx_l = calloc(n_apps_max, sizeof(esk8_ble_appc_ctx_t));
    if (!esk8_ble_appc_hndl.app_ctx_l)
        return ESK8_ERR_OOM;

    return ESK8_OK;
}


esk8_err_t
esk8_ble_appc_app_reg(
    esk8_ble_appc_t* app
)
{

}


esk8_err_t
esk8_ble_appc_deinit(
)
{
    if (!esk8_ble_appc_hndl.app_ctx_l)
        return ESK8_ERR_BLE_APPC_INIT_NOINIT;

    for (int i = 0; i < esk8_ble_appc_hndl.n_apps; i++)
    {
        // TODO: (b.covas)
    }
}

void
esk8_ble_appc_gattc_cb(
    esp_gattc_cb_event_t event,
    esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t* param
)
{

}


void
esk8_ble_appc_gap_cb(
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param
)
{

}
