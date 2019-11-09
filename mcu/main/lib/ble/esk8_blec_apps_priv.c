#include <esk8_log.h>
#include <esk8_blec_apps.h>

#include "esk8_blec_apps_priv.h"

#include <string.h>


int
esk8_blec_apps_get_dev_hndl(
    uint8_t dev_addr[6],
    esk8_blec_dev_hndl_t** out_dev_hndl
)
{
    for (int i=0; i< esk8_blec_apps.n_dev; i++)
    {
        esk8_blec_dev_hndl_t* dev_hndl = &esk8_blec_apps.dev_l[i];
        if (memcmp(dev_hndl->dev_p->addr, dev_addr, 6) == 0)
        {
            (*out_dev_hndl) = dev_hndl;
            return i;
        }
    }

    (*out_dev_hndl) = NULL;
    return -1;
}

void
esk8_blec_apps_gattc_cb(
    esp_gattc_cb_event_t event,
    esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t* param
)
{
    esk8_blec_app_hndl_t* app_hndl = NULL;
    int app_idx = -1;

    esk8_log_D(ESK8_TAG_BLE,
        "Got gattc event: %d\n",
        event
    );

    if (event == ESP_GATTC_REG_EVT)
    {
        if (param->reg.status)
        {
            esk8_log_E(ESK8_TAG_BLE,
                "gattc error in app_reg. Status: 0x%02x, name: '%s'\n",
                param->reg.status,
                app_hndl->app_p->app_name
            );
            goto skip_search;
        }

        app_idx = param->reg.app_id;
        app_hndl = &esk8_blec_apps.app_l[app_idx];
        app_hndl->gatt_if = gattc_if;

        esk8_log_I(ESK8_TAG_BLE,
            "Registered app %s, idx: %d, gattc_if: %d\n",
            app_hndl->app_p->app_name,
            app_idx,
            app_hndl->gatt_if
        );

        goto skip_search;
    }

    for (int i = 0; i < esk8_blec_apps.n_apps; i++)
    {
        if (esk8_blec_apps.app_l[i].gatt_if == gattc_if)
        {
            app_hndl = &esk8_blec_apps.app_l[i];
            app_idx = i;

            esk8_log_D(ESK8_TAG_BLE,
                "Gattc if resolved to app %s\n",
                app_hndl->app_p->app_name
            );

            break;
        }
    }

skip_search:

    if (!app_hndl)
    {
        esk8_log_W(ESK8_TAG_BLE,
            "Got gattc event %d with no associated app.\n", event
        );
        return;
    }

    switch (event)
    {
        case ESP_GATTC_CONNECT_EVT:
        {
            esk8_blec_dev_hndl_t* dev_hndl = NULL;
            esk8_blec_apps_get_dev_hndl(
                param->connect.remote_bda,
                &dev_hndl
            );

            if (!dev_hndl)
            {
                esk8_log_E(ESK8_TAG_BLE,
                    "Connected to unknown dev: " MACSTR ". Was given conn_id: %d. Bye.\n",
                    MAC2STR(param->connect.remote_bda),
                    param->connect.conn_id
                );
                esp_ble_gap_disconnect(param->connect.remote_bda);
                break;
            }

            dev_hndl->conn_id = param->connect.conn_id;
            dev_hndl->state = ESK8_BLE_DEV_CONNECTED;
            esk8_blec_apps.state = ESK8_BLEC_STATE_SEARCHING;

            esk8_log_I(ESK8_TAG_BLE,
                "Connected to %s " MACSTR "\n",
                dev_hndl->dev_p->name,
                MAC2STR(dev_hndl->dev_p->addr)
            );
            break;
        }

        case ESP_GATTC_OPEN_EVT:
        {
            esk8_blec_dev_hndl_t* dev_hndl = NULL;
            int dev_idx = esk8_blec_apps_get_dev_hndl(
                param->open.remote_bda,
                &dev_hndl
            );

            if (!dev_hndl)
            {
                esk8_log_E(ESK8_TAG_BLE,
                    "Opened gattc to an unknown dev: " MACSTR ". Was given conn_id: %d. Bye.\n",
                    MAC2STR(param->open.remote_bda),
                    param->open.conn_id
                );
                esp_ble_gap_disconnect(param->open.remote_bda);
                break;
            }

            if(param->open.status)
            {
                esk8_log_E(
                    ESK8_TAG_BLE,
                    "failed to open gattc to %s," MACSTR ". Status: 0x%02x\n",
                    dev_hndl->dev_p->name,
                    MAC2STR(param->open.remote_bda),
                    param->open.status
                );
                break;
            }

            esk8_log_I(ESK8_TAG_BLE,
                "(GATTC) Adding conn_id: %d (%s, " MACSTR ") to app '%s'\n",
                param->open.conn_id,
                dev_hndl->dev_p->name,
                MAC2STR(param->open.remote_bda),
                app_hndl->app_p->app_name
            );

            if (app_hndl->app_p->app_conn_add)
                app_hndl->app_p->app_conn_add(
                    dev_hndl->dev_p,
                    &app_hndl->conn_ctx_list[dev_idx]
                );
            break;
        }

        case ESP_GATTC_DISCONNECT_EVT:
        {
            esk8_blec_dev_hndl_t* dev_hndl = NULL;
            int dev_idx = esk8_blec_apps_get_dev_hndl(
                param->disconnect.remote_bda,
                &dev_hndl
            );

            if (!dev_hndl)
            {
                esk8_log_E(ESK8_TAG_BLE,
                    "Closed conn to an unknown dev: " MACSTR ". Was given conn_id: %d.\n",
                    MAC2STR(param->disconnect.remote_bda),
                    param->connect.conn_id
                );
                break;
            }

            if (dev_hndl->state != ESK8_BLE_DEV_CONNECTED)
            {
                esk8_log_E(ESK8_TAG_BLE,
                    "Got gattc event 'close' with a disconnected device: " MACSTR ". Was given conn_id: %d.\n",
                    MAC2STR(param->disconnect.remote_bda),
                    param->disconnect.conn_id
                );
                break;
            }

            esk8_log_I(ESK8_TAG_BLE,
                "Removing '%s' " MACSTR " from app '%s'\n",
                dev_hndl->dev_p->name,
                MAC2STR(param->disconnect.remote_bda),
                app_hndl->app_p->app_name
            );

            if (app_hndl->app_p->app_conn_del)
                app_hndl->app_p->app_conn_del(
                    dev_hndl->dev_p,
                    &app_hndl->conn_ctx_list[dev_idx]
                );

            dev_hndl->conn_id = -1;
            dev_hndl->state = ESK8_BLE_DEV_NOTFOUND;

            break;
        }
        default:
            break;
    }

    esk8_log_I(ESK8_TAG_BLE,
        "(GATTC) Got event %d, passing to app '%s'\n",
        event, app_hndl->app_p->app_name
    );

    if (app_hndl->app_p->app_evt_cb)
        app_hndl->app_p->app_evt_cb(event, param);
}


void
esk8_blec_apps_gap_cb(
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param
)
{
    esk8_log_D(ESK8_TAG_BLE,
        "Got GAP event: %d\n", event
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
            // TODO: fix this.
            dev_name[ble_name_len] = 0;

        esk8_log_I(ESK8_TAG_BLE,
            "Found '%s': " MACSTR "\n",
            dev_name,
            MAC2STR(param->scan_rst.bda)
        );

        for (int i = 0; i < esk8_blec_apps.n_dev; i++)
        {
            esk8_blec_dev_hndl_t* dev_hndl = NULL;
            esk8_blec_apps_get_dev_hndl(
                param->scan_rst.bda,
                &dev_hndl
            );

            if  (
                    dev_hndl &&
                    strncmp(dev_hndl->dev_p->name, (char*)dev_name, strlen(dev_hndl->dev_p->name)) == 0
                )
            {
                if (dev_hndl->state > ESK8_BLE_DEV_NOTFOUND)
                    break;

                esk8_log_I(ESK8_TAG_BLE,
                    "Connecting to '%s'\n",
                    dev_name
                );

                dev_hndl->state = ESK8_BLE_DEV_CONNECTING;
                esk8_blec_apps.state = ESK8_BLEC_STATE_CONNECTING;

                esp_ble_gap_stop_scanning();

                for (int j=0; j<esk8_blec_apps.n_apps; j++)
                {
                    esk8_log_I(ESK8_TAG_BLE,
                        "Openning gattc to dev %s " MACSTR ", app idx %d (%s), on gattc_if %d\n",
                        dev_hndl->dev_p->name,
                        MAC2STR(dev_hndl->dev_p->addr),
                        j,
                        esk8_blec_apps.app_l[j].app_p->app_name,
                        esk8_blec_apps.app_l[j].gatt_if
                    );

                    esp_ble_gattc_open(
                        esk8_blec_apps.app_l[j].gatt_if,
                        (uint8_t*)dev_hndl->dev_p->addr,
                        param->scan_rst.ble_addr_type,
                        true
                    );
                }
            }
        }

        break;
    }
    
    default:
        break;
    }
}
