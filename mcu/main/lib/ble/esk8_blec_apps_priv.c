#include <esk8_log.h>
#include <esk8_blec_apps.h>

#include "esk8_blec_apps_priv.h"

#include <string.h>

#define MAC_STR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_STR_PARAM(X) X[0], X[1], X[2], X[3], X[4], X[5], X[6]


esk8_blec_dev_t*
esk8_blec_apps_get_dev(
    uint8_t addr[6]
)
{
    for (int i=0; i< esk8_blec_apps.n_dev; i++)
    {
        esk8_blec_dev_t* dev = esk8_blec_apps.dev_list[i];
        if (memcmp(dev->addr, addr, 6) == 0)
            return dev;
    }

    return NULL;
}

void
esk8_blec_apps_gattc_cb(
    esp_gattc_cb_event_t event,
    esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t* param
)
{
    esk8_blec_app_t* app = NULL;
    int app_idx = -1;

    esk8_log_D(ESK8_TAG_BLE,
        "(GATTC) Got event: %d\n",
        event
    );

    if (event == ESP_GATTC_REG_EVT)
    {
        app_idx = param->reg.app_id;
        app = esk8_blec_apps.app_list[app_idx];

        if (param->reg.status)
        {
            esk8_log_E(ESK8_TAG_BLE,
                "(GATTC) Error in app_reg. Status: 0x%02x, name: '%s'\n",
                param->reg.status,
                app->app_name
            );
            goto skip_search;
        }

        esk8_blec_apps.app_ctx_list[param->reg.app_id].gattc_if = gattc_if;
        goto skip_search;
    }

    for (int i = 0; i < esk8_blec_apps.n_apps; i++)
    {
        if (esk8_blec_apps.app_ctx_list[i].gattc_if == gattc_if)
        {
            app = esk8_blec_apps.app_list[i];
            app_idx = i;
            break;
        }
    }

skip_search:

    if (!app)
    {
        esk8_log_W(ESK8_TAG_BLE,
            "(GATTC) Got event %d with no associated app.\n", event
        );
        return;
    }

    switch (event)
    {
        case ESP_GATTC_CONNECT_EVT:

            if (esk8_blec_apps.state == ESK8_BLEC_STATE_SEARCHING)
                esp_ble_gap_start_scanning(~0);

            esk8_blec_dev_t* dev = esk8_blec_apps_get_dev(
                param->connect.remote_bda
            );

            if (!dev)
            {
                esk8_log_E(ESK8_TAG_BLE,
                    "Connected to unknown dev: " MAC_STR ". Disconnecting.\n",
                    MAC_STR_PARAM(param->connect.remote_bda)
                );

                esp_ble_gap_disconnect(param->connect.remote_bda);
                break;
            }

            dev->state = ESK8_BLE_DEV_CONNECTED;
            break;

        case ESP_GATTC_DISCONNECT_EVT:
            for (int i = 0; i < esk8_blec_apps.n_conn_max; i++)
            {
                esk8_blec_conn_ctx_t* ctx = &esk8_blec_apps.app_ctx_list[i];
                if (param->disconnect.conn_id == ctx->conn_id)
                {
                    esk8_log_I(ESK8_TAG_BLE,
                        "(GATTC) Disconnected: " MAC_STR ", conn_id: %d\n",
                        MAC_STR_PARAM(param->disconnect.remote_bda),
                        param->disconnect.conn_id
                    );

                    app->app_conn_del(ctx);
                    ctx->conn_id = -1;
                }
            }

            if (esk8_blec_apps.state == ESK8_BLEC_STATE_SEARCHING)
                esp_ble_gap_start_scanning(~0);

            break;

        case ESP_GATTC_OPEN_EVT:


            if(param->open.status)
            {
                esk8_log_E(
                    ESK8_TAG_BLE,
                    "(GATTC) gattc failed to open. Status: 0x%02x\n",
                    param->open.status
                );
                break;
            }

            for (int i = 0; i < esk8_blec_apps.n_conn_max; i++)
            {
                esk8_blec_conn_ctx_t* ctx = &esk8_blec_apps.app_ctx_list[i];
                if (ctx->conn_id < 0)
                {
                    esk8_log_I(ESK8_TAG_BLE,
                        "(GATTC) Added conn_id: %d to app '%s'\n",
                        param->open.conn_id,
                        app->app_name
                    );

                    ctx->conn_id = param->connect.conn_id;
                    ctx->gattc_if = gattc_if;
                    app->app_conn_add(ctx);
                    break;
                }
            }

            break;

        default:
            break;
    }

    esk8_log_I(ESK8_TAG_BLE,
        "(GATTC) Got event %d, passing to app '%s'\n",
        event, app->app_name
    );

    if (app->app_evt_cb)
        app->app_evt_cb(event, param);
}


void
esk8_blec_apps_gap_cb(
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param
)
{
    esk8_log_D(ESK8_TAG_BLE,
        "(GAP) Got event: %d\n", event
    );

    switch (event)
    {
    case ESP_GAP_BLE_SCAN_RESULT_EVT:
    {
        uint8_t  ble_name_len = 0;
        uint8_t* dev_name = esp_ble_resolve_adv_data(
            param->scan_rst.ble_adv,
            ESP_BLE_AD_TYPE_NAME_CMPL,
            &ble_name_len
        );

        if (!dev_name)
        {
            dev_name = (uint8_t*)"(no name)";
            ble_name_len = strlen((const char*)dev_name);
        }
        else
            dev_name[ble_name_len] = 0;

        esk8_log_I(ESK8_TAG_BLE,
            "Found '%s': " MAC_STR "\n",
            dev_name,
            MAC_STR_PARAM(param->scan_rst.bda)
        );

        for (int i = 0; i < esk8_blec_apps.n_dev; i++)
        {
            esk8_blec_dev_t* dev = esk8_blec_apps_get_dev(param->scan_rst.bda);
            if  (
                    dev &&
                    strncmp(dev->name, (char*)dev_name, ble_name_len) == 0
                )
            {
                if (dev->state > ESK8_BLE_DEV_NOTFOUND)
                    break;

                esk8_log_I(ESK8_TAG_BLE,
                    "Connecting to '%s'\n",
                    dev_name
                );

                dev->state = ESK8_BLE_DEV_CONNECTING;
                esk8_blec_apps.state = ESK8_BLEC_STATE_CONNECTING;

                esp_ble_gap_stop_scanning();
                esp_ble_gattc_open(
                    esk8_blec_apps.app_ctx_list[0].gattc_if,
                    dev->addr,
                    param->scan_rst.ble_addr_type,
                    true
                );
            }
        }

        break;
    }
    
    default:
        break;
    }
}
