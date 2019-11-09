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
    esk8_blec_apps = { 0 };

esk8_err_t
esk8_blec_apps_init(
    uint n_apps_max,
    uint n_dev_max
)
{
    if (esk8_blec_apps.state)
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
    esp_ble_gap_set_scan_params(&esk8_blec_scan_params);
    esp_ble_gap_register_callback(esk8_blec_apps_gap_cb);
    esp_ble_gattc_register_callback(esk8_blec_apps_gattc_cb);

    esk8_blec_apps = (esk8_blec_apps_t) { 0 };

    esk8_blec_apps.app_l = calloc(
        n_apps_max,
        sizeof(esk8_blec_app_hndl_t)
    );

    esk8_blec_apps.dev_l = calloc(
        n_dev_max,
        sizeof(esk8_blec_dev_hndl_t)
    );

    err = ESK8_ERR_OOM;

    if  (
            !esk8_blec_apps.app_l ||
            !esk8_blec_apps.dev_l
        )
        goto fail;

    for (int i=0; i<n_apps_max; i++)
    {
        esk8_blec_apps.app_l[i].conn_ctx_list = calloc(
            n_dev_max,
            sizeof(void*)
        );

        if (!esk8_blec_apps.app_l[i].conn_ctx_list)
            goto fail;
    }

    esk8_log_D(ESK8_TAG_BLE,
        "Allocated mem. for %d apps and %d dev.\n",
        n_apps_max,
        n_dev_max
    );

    for (int i=0; i < n_dev_max; i++)
    {
        esk8_blec_apps.dev_l[i].conn_id = -1;
        esk8_blec_apps.dev_l[i].state = 0;
        esk8_blec_apps.dev_l[i].dev_p = NULL;
    }


    esk8_blec_apps.n_apps = 0;
    esk8_blec_apps.n_dev = 0;
    esk8_blec_apps.n_apps_max = n_apps_max;
    esk8_blec_apps.n_dev_max = n_dev_max;
    esk8_blec_apps.state = ESK8_BLEC_STATE_INIT;

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
    uint* app_n = &esk8_blec_apps.n_apps;

    if (*app_n >= esk8_blec_apps.n_apps_max)
        return ESK8_ERR_BLE_APPC_INIT_MAXREG;

    esk8_blec_apps.app_l[*app_n].app_p = app;

    if (app->app_init)
        app->app_init();

    esp_ble_gattc_app_register(*app_n);
    (*app_n)++;

    return ESK8_OK;
}


void
esk8_blec_apps_deinit()
{
    if (esk8_blec_apps.app_l)
    {
        for (int i = 0; i < esk8_blec_apps.n_apps; i++)
        {
            esk8_blec_app_hndl_t* app_hndl = &esk8_blec_apps.app_l[i];
            if (app_hndl->app_p->app_deinit)
                app_hndl->app_p->app_deinit();
            if (app_hndl->conn_ctx_list)
                free(app_hndl->conn_ctx_list);
        }

        free(esk8_blec_apps.app_l);
    }

    if (esk8_blec_apps.dev_l)
        free(esk8_blec_apps.dev_l);

    esk8_blec_apps = (esk8_blec_apps_t) { 0 };
}


esk8_err_t
esk8_blec_apps_dev_reg(
    esk8_blec_dev_t* dev
)
{
    uint* n_dev = &esk8_blec_apps.n_dev;

    if (*n_dev >= esk8_blec_apps.n_dev_max)
        return ESK8_ERR_BLE_APPC_DEV_MAXREG;

    esk8_blec_dev_hndl_t* dev_hndl = &esk8_blec_apps.dev_l[*n_dev];

    dev_hndl->dev_p = dev;
    dev_hndl->state = ESK8_BLE_DEV_NOTFOUND;
    dev_hndl->conn_id = -1;

    ++(*n_dev);

    esk8_log_I(ESK8_TAG_BLE,
        "Registered: %s, " MACSTR "\n",
        dev->name,
        MAC2STR(dev->addr)
    );

    return ESK8_OK;
}


esk8_err_t
esk8_blec_search_start(
)
{
    esp_err_t err = esp_ble_gap_start_scanning(~0);
    if (err)
    {
        esk8_log_E(ESK8_TAG_BLE,
            "Err: '%s' (%d) starting scan.\n",
            esp_err_to_name(err),
            err
        );
        return ESK8_ERR_BLE_APPC_SCAN_FAILED;
    }

    esk8_log_I(ESK8_TAG_BLE,
        "Started scanning.\n"
    );

    esk8_blec_apps.state = ESK8_BLEC_STATE_SEARCHING;
    return ESK8_OK;
}


esk8_err_t
esk8_blec_search_stop(
)
{
    if (!esk8_blec_apps.state)
        return ESK8_ERR_BLE_APPC_INIT_NOINIT;

    switch (esk8_blec_apps.state)
    {
    case ESK8_BLEC_STATE_SEARCHING:
        esk8_blec_apps.state = ESK8_BLEC_STATE_RUNNING;
        esp_ble_gap_stop_scanning();
        esk8_log_I(ESK8_TAG_BLE,"Stopped scanning.\n");
        break;
    
    case ESK8_BLEC_STATE_CONNECTING:
        esk8_log_W(ESK8_TAG_BLE,
            "Tried to stop scanning, but was connecting. Wait.\n"
        );
        break;

    default:
        esk8_log_W(ESK8_TAG_BLE,
            "Tried to stop scanning without an active scan.\n"
        );
        break;
    }

    return ESK8_OK;
}


esk8_err_t
esk8_blec_dscn(
)
{
    if (!esk8_blec_apps.state)
        return ESK8_ERR_BLE_APPC_INIT_NOINIT;

    for (int i=0; i<esk8_blec_apps.n_dev; i++)
    {
        esk8_blec_dev_hndl_t* dev_hndl = &esk8_blec_apps.dev_l[i];
        if (dev_hndl->state >= ESK8_BLE_DEV_CONNECTING)
        {
            esk8_log_D(ESK8_TAG_BLE,
                "Disconnecting %s: " MACSTR "\n",
                dev_hndl->dev_p->name,
                MAC2STR(dev_hndl->dev_p->addr)
            );

            esp_ble_gap_disconnect((uint8_t*)dev_hndl->dev_p->addr);
        }
    }

    return ESK8_OK;
}
