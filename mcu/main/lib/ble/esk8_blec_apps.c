#include <esk8_err.h>
#include <esk8_log.h>
#include <esk8_blec_apps.h>

#include "esk8_blec_apps_priv.h"

#include <esp_bt.h>
#include <nvs_flash.h>
#include <esp_bt_main.h>
#include <esp_gattc_api.h>
#include <esp_gap_ble_api.h>


esk8_blec_apps_hndl_t
    esk8_ble_appc_hndl = { 0 };

esk8_err_t
esk8_blec_apps_init(
    uint n_apps_max,
    uint n_conn_max
)
{
    if (esk8_ble_appc_hndl.app_ctx_list)
        return ESK8_ERR_BLE_APPC_INIT_REINIT;

    esk8_err_t err;
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
    esp_ble_gap_register_callback(esk8_blec_apps_gap_cb);
    esp_ble_gattc_register_callback(esk8_blec_apps_gattc_cb);

    esk8_ble_appc_hndl = (esk8_blec_apps_hndl_t) { 0 };

    esk8_ble_appc_hndl.app_list = calloc(
        n_apps_max,
        sizeof(esk8_blec_app_t*)
    );

    esk8_ble_appc_hndl.app_ctx_list = calloc(
        n_apps_max * n_conn_max,
        sizeof(esk8_blec_conn_ctx_t)
    );

    if  (
            !esk8_ble_appc_hndl.app_list ||
            !esk8_ble_appc_hndl.app_ctx_list
        )
    {
        err = ESK8_ERR_OOM;
        goto fail;
    }

    esk8_ble_appc_hndl.n_apps_max = n_apps_max;
    esk8_ble_appc_hndl.n_conn_max = n_conn_max;
    esk8_ble_appc_hndl.n_apps = 0;

    esp_ble_gap_start_scanning(~0);
    return ESK8_OK;

fail:
    esk8_blec_apps_deinit();
    return err;
}


esk8_err_t
esk8_blec_apps_app_reg(
    esk8_blec_app_t* app
)
{
    uint* app_n = &esk8_ble_appc_hndl.n_apps;

    if (*app_n == esk8_ble_appc_hndl.n_apps_max)
        return ESK8_ERR_BLE_APPC_INIT_MAXREG;

    esk8_ble_appc_hndl.app_list[*app_n] = app;

    if (app->app_init)
        app->app_init();

    esp_ble_gattc_app_register(*app_n);

    (*app_n)++;
    return ESK8_OK;
}


void
esk8_blec_apps_deinit()
{
    if (esk8_ble_appc_hndl.app_list)
    {
        for (int i = 0; i < esk8_ble_appc_hndl.n_apps; i++)
            if (esk8_ble_appc_hndl.app_list[i]->app_deinit)
                esk8_ble_appc_hndl.app_list[i]->app_deinit();

        free(esk8_ble_appc_hndl.app_list);
    }

    if (esk8_ble_appc_hndl.app_ctx_list)
        free(esk8_ble_appc_hndl.app_ctx_list);
}
