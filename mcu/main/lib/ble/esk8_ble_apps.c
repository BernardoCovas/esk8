#include <esk8_ble_apps.h>

#include "esk8_ble_apps_util.h"

#include <esk8_log.h>
#include <esk8_config.h>
#include <esk8_nvs.h>

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>
#include <nvs_flash.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


void
esk8_ble_apps_gap_evt_hndl(
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param);

void
esk8_ble_apps_gatts_evt_hndl(
    esp_gatts_cb_event_t event,
    esp_gatt_if_t gatts_if,
    esp_ble_gatts_cb_param_t *param);

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = ESP_BLE_APPEARANCE_GENERIC_PERSONAL_MOBILITY_DEVICE, /* Oh yes */
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 0,
    .p_service_uuid = NULL,
    .flag = 0,
};

esk8_ble_apps_t esk8_ble_apps = {0};

esk8_err_t
esk8_ble_apps_init(
    unsigned int        n_apps_max,
    unsigned int        n_conn_max
)
{
    if (esk8_ble_apps.apps_list)
        return ESK8_ERR_BLE_INIT_REINIT;

    /**
     * NOTE: (b.covas) NVS Has to be initialized
     * for BLE to work. If NVS was already initialized,
     * nvs_flash_init() will still return OK.
     */
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    if (ret)
        return ESK8_ERR_NVS_NOT_AVAILABLE;

    esp_bt_controller_config_t bt_Cnfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    bt_Cnfg.scan_duplicate_mode = 1;
    bt_Cnfg.scan_duplicate_type = 1;

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT)           );
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_Cnfg)                                );
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE)                       );
    ESP_ERROR_CHECK(esp_bluedroid_init()                                            );
    ESP_ERROR_CHECK(esp_bluedroid_enable()                                          );
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(esk8_ble_apps_gap_evt_hndl)       );
    ESP_ERROR_CHECK(esp_ble_gap_set_device_name(ESK8_BLE_DEV_NAME)                  );
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(esk8_ble_apps_gatts_evt_hndl)   );
    ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&adv_data)                          );

    ESK8_ERRCHECK_THROW(esk8_nvs_init());

    esk8_ble_apps.apps_num_max = n_apps_max;
    esk8_ble_apps.conn_num_max = n_conn_max;

    esk8_ble_apps.apps_list = calloc(n_apps_max, sizeof(esk8_ble_app_t*));
    if (!esk8_ble_apps.apps_list)
        return ESK8_ERR_OOM;

    return ESK8_OK;
}

esk8_err_t
esk8_ble_app_register(
    esk8_ble_app_t* app
)
{
    if (!esk8_ble_apps.apps_list)
        return ESK8_ERR_BLE_INIT_NOINIT;

    for (int i = 0; i < esk8_ble_apps.apps_num_max; i++)
    {
        esk8_ble_app_t** _app = &esk8_ble_apps.apps_list[i];
        if (*_app)
            continue;

        (*_app) = app;

        app->_attr_hndl_list = calloc(
            app->attr_num, sizeof(*app->_attr_hndl_list));

        if (!app->_attr_hndl_list)
            return ESK8_ERR_OOM;

        app->_conn_ctx_list = calloc(
            esk8_ble_apps.conn_num_max, sizeof(esk8_ble_conn_ctx_t));

        if (!app->_conn_ctx_list)
        {
            free(app->_attr_hndl_list);
            return ESK8_ERR_OOM;
        }

        for (int i = 0; i < esk8_ble_apps.conn_num_max; i++)
            app->_conn_ctx_list[i].conn_id = -1;

        ESP_ERROR_CHECK(esp_ble_gatts_app_register(esk8_ble_apps.curr_app_id++));
        esk8_log_D(ESK8_TAG_BLE, "Registered app: '%s'\n", app->app_name);

        return ESK8_OK;
    }

    return ESK8_ERR_BLE_INIT_MAXREG;
}

esk8_err_t
esk8_ble_apps_deinit()
{
    if (!esk8_ble_apps.apps_list)
        return ESK8_ERR_BLE_INIT_NOINIT;

    for (int i = 0; i < esk8_ble_apps.apps_num_max; i++)
    {
        esk8_ble_app_t* app = esk8_ble_apps.apps_list[i];

        if (!app)
            continue;

        for (int j = 0; j < esk8_ble_apps.conn_num_max; j++)
        {
            int conn_id = app->_conn_ctx_list[j].conn_id;
            if(conn_id < 0)
                continue;

            app->app_conn_del(&app->_conn_ctx_list[j]);
        }

        app->app_deinit();
        free(app->_attr_hndl_list);
        free(app->_conn_ctx_list);

        esk8_ble_apps.apps_list[i] = NULL;
    }

    free(esk8_ble_apps.apps_list);
    memset(&esk8_ble_apps, 0, sizeof(esk8_ble_apps_t));

    return ESK8_OK;
}

void
esk8_ble_apps_gap_evt_hndl(
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param
)
{
    switch (event)
    {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&adv_params));
            esk8_log_D(ESK8_TAG_BLE, "Started advertizing.\n");
            break;

        default:
            break;
    }
}

void
esk8_ble_apps_gatts_evt_hndl(
    esp_gatts_cb_event_t event,
    esp_gatt_if_t gatts_if,
    esp_ble_gatts_cb_param_t *param
)
{
    // Fist switch acts on pre-app events
    switch (event)
    {
        case ESP_GATTS_REG_EVT:
        {
            if (param->reg.app_id >= esk8_ble_apps.apps_num_max)
                return;

            esk8_ble_app_t* app = esk8_ble_apps.apps_list[param->reg.app_id];
            if (!app)
                return;

            app->_ble_if = gatts_if;

            ESP_ERROR_CHECK(esp_ble_gatts_create_attr_tab(
                app->attr_db,
                gatts_if,
                app->attr_num,
                0
            ));

            return;
        }

        default:
            break;
    }

    esk8_ble_app_t* app = NULL;
    for (int i = 0; i < esk8_ble_apps.apps_num_max; i++)
    {
        esk8_ble_app_t* _app = esk8_ble_apps.apps_list[i];
        if (!_app)
            continue;

        if (_app->_conn_ctx_list && _app->_ble_if == gatts_if)
        {
            app = _app;
            break;
        }
    }

    if (!app)
        return;

    // Second switch only deals with events associated with apps
    switch (event)
    {
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        {
            if (param->add_attr_tab.status != ESP_OK)
            {
                esk8_log_D(ESK8_TAG_BLE, "Error creating table for app: '%s'. Status: %d\n",
                    app->app_name,
                    param->add_attr_tab.status);

                return;
            }

            if (param->add_attr_tab.num_handle != app->attr_num)
            {
                esk8_log_D(ESK8_TAG_BLE,
                    "Table created but the returned number of handles is %d. Expected %d instead.\n",
                    param->add_attr_tab.num_handle,
                    app->attr_num
                );
                return;
            }

            memcpy(
                app->_attr_hndl_list,
                param->add_attr_tab.handles,
                sizeof(*app->_attr_hndl_list) * app->attr_num
            );

            app->app_init();
            esp_ble_gatts_start_service(app->_attr_hndl_list[0]); /* Service is idx 0 */

            return;
        }

        case ESP_GATTS_CONNECT_EVT:
        {
            ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&adv_params));

            esk8_ble_conn_ctx_t* ctx = NULL;
            for (int i = 0; i < esk8_ble_apps.conn_num_max; i++)
            {
                if (app->_conn_ctx_list[i].conn_id < 0)
                {
                    ctx = &app->_conn_ctx_list[i];
                    app->_conn_ctx_list[i].conn_id = param->connect.conn_id;

                    break;
                }
            }

            if (!ctx)
            {
                esk8_log_D(ESK8_TAG_BLE,
                    "Got new connection, but ctx list is full, "
                    "app: '%s'\n", app->app_name);

                esp_ble_gap_disconnect(param->connect.remote_bda);
                break;
            }

            esk8_log_D(ESK8_TAG_BLE,
                "Adding conn id %d to app %s.\n",
                param->connect.conn_id,
                app->app_name);

            app->app_conn_add(ctx);
            break;
        }

        case ESP_GATTS_DISCONNECT_EVT:
        {
            ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&adv_params));

            for (int i = 0; i < esk8_ble_apps.conn_num_max; i++)
                if (param->disconnect.conn_id == app->_conn_ctx_list[i].conn_id)
                {
                    esk8_log_D(ESK8_TAG_BLE,
                        "Removing conn id: %d\n",
                        param->disconnect.conn_id
                    );

                    app->app_conn_del(&app->_conn_ctx_list[i]);
                    app->_conn_ctx_list[i].conn_id = -1;
                    break;
                }

            break;
        }

        case ESP_GATTS_WRITE_EVT:
        {
            int attr_idx;
            esk8_err_t err_code = esk8_ble_apps_get_attr_idx(
                app, param->write.handle, &attr_idx);

            if (err_code)
            {
                esk8_log_D(ESK8_TAG_BLE,
                    "Got err: '%s' getting the attr idx, "
                    "on write event of app: '%s'\n",
                    esk8_err_to_str(err_code), app->app_name);
                break;
            }

            esk8_ble_conn_ctx_t* ctx;
            err_code = esk8_ble_apps_get_ctx(
                app, param->write.conn_id, &ctx);

            if (err_code)
            {
                esk8_log_D(ESK8_TAG_BLE,
                    "Got err: '%s' getting the conn ctx, "
                    "on write event of app: '%s'\n",
                    esk8_err_to_str(err_code), app->app_name);
                break;
            }

            app->app_conn_write(
                ctx, attr_idx, param->write.len,
                param->write.value);

            break;
        }

        default:
            break;
    }

    app->app_evt_cb(event, param);
}

