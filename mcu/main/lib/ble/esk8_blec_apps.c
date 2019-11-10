#include <esk8_err.h>
#include <esk8_log.h>
#include <esk8_blec_apps.h>

#include "esk8_blec_apps_priv.h"

#include <esp_bt.h>
#include <nvs_flash.h>
#include <esp_bt_main.h>
#include <esp_gattc_api.h>
#include <esp_gap_ble_api.h>


static esp_ble_scan_params_t
esk8_blec_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_ENABLE
};

esk8_blec_apps_t
esk8_blec_apps;

esk8_err_t
esk8_blec_init(
    uint16_t app_n_max
)
{
    if (esk8_blec_apps.state)
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
    esp_ble_gap_set_scan_params(&esk8_blec_scan_params);
    esp_ble_gap_register_callback(esk8_blec_apps_gap_cb);
    esp_ble_gattc_register_callback(esk8_blec_apps_gattc_cb);

    esk8_blec_apps = (esk8_blec_apps_t) { 0 };

    esk8_blec_apps.app_hndl_l = calloc(
        app_n_max, sizeof(esk8_blec_app_hndl_t)
    );

    if (!esk8_blec_apps.app_hndl_l)
        return ESK8_ERR_OOM;

    esk8_blec_apps.app_n_max = app_n_max;
    for (int i = 0; i<esk8_blec_apps.app_n_max; i++)
        esk8_blec_apps.app_hndl_l[i].gattc_if = -1;

    esk8_blec_apps.state |= ESK8_BLEC_APP_HNDL_STATE_INIT;
    return ESK8_OK;
}


esk8_err_t
esk8_blec_deinit(
)
{
    if (esk8_blec_apps.app_hndl_l)
        free(esk8_blec_apps.app_hndl_l);

    return ESK8_OK;
}

esk8_err_t
esk8_blec_app_reg(
    esk8_blec_app_t* app_p,
    esk8_blec_dev_t* dev_p
)
{
    if (!(esk8_blec_apps.state & ESK8_BLEC_APPS_STATE_INIT))
        return ESK8_ERR_BLE_APPC_INIT_NOINIT;
    
    int app_n = esk8_blec_apps.app_n;
    if (app_n >= esk8_blec_apps.app_n_max)
        return ESK8_ERR_BLE_APPC_MAXREG;

    if (app_p->app_init)
        app_p->app_init();

    esk8_blec_apps.app_hndl_l[app_n].app_p = app_p;
    esk8_blec_apps.app_hndl_l[app_n].dev_p = dev_p;
    esk8_blec_apps.app_hndl_l[app_n].state |= ESK8_BLEC_APP_HNDL_STATE_INIT;

    esp_ble_gattc_app_register(app_n);

    esk8_blec_apps.app_n++;
    return ESK8_OK;
}

esk8_err_t
esk8_blec_search_start(
)
{
    if (!(esk8_blec_apps.state & ESK8_BLEC_APPS_STATE_INIT))
        return ESK8_ERR_BLE_APPC_INIT_NOINIT;

    if (esk8_blec_apps.state & ESK8_BLEC_APPS_STATE_SEARCHING)
    {
        esk8_log_W(ESK8_TAG_BLE,
            "Tried to start scanning with an active scan\n"
        );
        return ESK8_OK;
    }

    esp_ble_gap_start_scanning(ESK8_CONFIG_BLEC_SCAN_DURATION_s);
    return ESK8_OK;
}

esk8_err_t
esk8_blec_search_stop(
)
{
    if (!(esk8_blec_apps.state & ESK8_BLEC_APPS_STATE_INIT))
        return ESK8_ERR_BLE_APPC_INIT_NOINIT;

    if (esk8_blec_apps.state & ESK8_BLEC_APPS_STATE_SEARCHING)
    {
        esp_ble_gap_stop_scanning();
        return ESK8_OK;
    }

    esk8_log_W(ESK8_TAG_BLE,
        "Tried to stop scanning without an active scan.\n"
    );

    return ESK8_OK;

}

esk8_err_t
esk8_blec_conn(
)
{
    if (!(esk8_blec_apps.state & ESK8_BLEC_APPS_STATE_INIT))
        return ESK8_ERR_BLE_APPC_INIT_NOINIT;
    if (esk8_blec_apps.state & ESK8_BLEC_APPS_STATE_SEARCHING)
        esp_ble_gap_stop_scanning();

    for (int i = 0; i < esk8_blec_apps.app_n; i++)
    {
        esk8_blec_app_hndl_t* app_hndl = &esk8_blec_apps.app_hndl_l[i];
        esk8_log_I(ESK8_TAG_BLE,
            "Openning gattc to %s " MACSTR ", with app %s\n",
            app_hndl->dev_p->name,
            MAC2STR(app_hndl->dev_p->addr),
            app_hndl->app_p->app_name
        );

        esp_ble_gattc_open(
            app_hndl->gattc_if,
            app_hndl->dev_p->addr,
            BLE_ADDR_TYPE_PUBLIC,
            true
        );
    }
}

esk8_err_t
esk8_blec_close(
)
{
    if (!(esk8_blec_apps.state & ESK8_BLEC_APPS_STATE_INIT))
        return ESK8_ERR_BLE_APPC_INIT_NOINIT;
    if (esk8_blec_apps.state & ESK8_BLEC_APPS_STATE_SEARCHING)
        esp_ble_gap_stop_scanning();

    for (int i = 0; i < esk8_blec_apps.app_n; i++)
    {
        // TODO
    }

    return ESK8_OK;
}