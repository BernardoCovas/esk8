#include <esk8_log.h>
#include <esk8_blec_apps.h>
#include "esk8_blec_apps_priv.h"

#include <esp_log.h>

#include <string.h>


void
esk8_blec_apps_gap_cb(
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param
)
{
    esk8_log_D(ESK8_TAG_BLE,
        "got gap event: %d\n",
        event
    );

    switch (event)
    {
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        if (param->scan_start_cmpl.status)
        {
            esk8_log_E(ESK8_TAG_BLE,
                "Scan failed to start. Status: %d\n",
                param->scan_start_cmpl.status
            );
            break;
        }
        esk8_blec_apps.state |= ESK8_BLEC_APPS_STATE_SEARCHING;
        esk8_log_I(ESK8_TAG_BLE,
            "Scan started.\n"
        );
        break;

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status)
            esk8_log_W(ESK8_TAG_BLE,
                "Got status: %d in scan stop\n",
                param->scan_stop_cmpl.status
            );

        esk8_blec_apps.state &= ~ESK8_BLEC_APPS_STATE_SEARCHING;
        esk8_log_I(ESK8_TAG_BLE, "Scan stopped.\n");
        break;

    case ESP_GAP_BLE_SCAN_RESULT_EVT:
    {
        if (param->scan_rst.search_evt)
            esk8_log_D(ESK8_TAG_BLE,
                "Scan scan_rst type: %d\n",
                param->scan_rst.search_evt
            );

        if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_CMPL_EVT)
        {
            esk8_blec_apps.state &= ~ESK8_BLEC_APPS_STATE_SEARCHING;
            esk8_log_I(ESK8_TAG_BLE, "Discovery complete.\n");
            break;
        }

        uint8_t  dev_name_len = 0;
        uint8_t* dev_name = esp_ble_resolve_adv_data(
            param->scan_rst.ble_adv,
            ESP_BLE_AD_TYPE_NAME_CMPL,
            &dev_name_len
        );

        if (dev_name_len == 0)
        {
            esk8_log_I(ESK8_TAG_BLE,
                "Found: " MACSTR " with no name.\n",
                MAC2STR(param->scan_rst.bda)
            );
            break;
        }


        if (dev_name_len >= 63) {
            esk8_log_E(ESK8_TAG_BLE,
                "Device name too long. This should never happen, an "
                "advertisement packet is smaller than this. Length: %d\n",
                dev_name_len
            );
            break;
        }
        char dev_name_str[64] = { 0 };
        memcpy(dev_name_str, dev_name, dev_name_len);

        esk8_blec_app_hndl_t* app_hndl = NULL;
        for (int i = 0; i < esk8_blec_apps.app_n; i++) {
            esk8_blec_app_hndl_t* _app_hndl = &esk8_blec_apps.app_hndl_l[i];
            if  (
                    strcmp(_app_hndl->dev_p->name, dev_name_str) == 0 &&
                    memcmp(_app_hndl->dev_p->addr, param->scan_rst.bda, 6) == 0
                )
            {
                app_hndl = _app_hndl;
                break;
            }
        }

        if (!app_hndl) {
            esk8_log_I(ESK8_TAG_BLE,
                "Found unkown dev: '%s', " MACSTR "\n",
                dev_name_str,
                MAC2STR(param->scan_rst.bda)
            );
            break;
        }

        app_hndl->state |= ESK8_BLEC_APP_HNDL_STATE_DEV_FOUND;

        esk8_log_I(ESK8_TAG_BLE,
            "Opening gattc to '%s', on if %d\n",
            dev_name_str,
            app_hndl->gattc_if
        );

        esp_ble_gattc_open(
            app_hndl->gattc_if,
            (uint8_t*)app_hndl->dev_p->addr,
            BLE_ADDR_TYPE_PUBLIC,
            true
        );

        break;
    }

    default:
        break;
    }
}


void esk8_blec_apps_gattc_cb(
    esp_gattc_cb_event_t event,
    esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t *param
)
{
    esk8_log_D(ESK8_TAG_BLE,
        "got gattc event: %d\n",
        event
    );

    esk8_blec_app_hndl_t* app_hndl = NULL;

    if (event == ESP_GATTC_REG_EVT)
        goto skip_search;

    for (int i=0; i<esk8_blec_apps.app_n; i++)
        if (gattc_if == esk8_blec_apps.app_hndl_l[i].gattc_if) {
            app_hndl = &esk8_blec_apps.app_hndl_l[i];
            goto skip_search;
        }

    esk8_log_E(ESK8_TAG_BLE,
        "Got event %d, with no associated app\n",
        event
    );

    return;

skip_search:

    switch (event)
    {
    case ESP_GATTC_REG_EVT:
        app_hndl = &esk8_blec_apps.app_hndl_l[param->reg.app_id];
        app_hndl->gattc_if = gattc_if;
        app_hndl->state |= ESK8_BLEC_APP_HNDL_STATE_REG;

        esk8_log_I(ESK8_TAG_BLE,
            "Registered app '%s'\n",
            app_hndl->app_p->app_name
        );

        if (app_hndl->app_p->app_init)
            app_hndl->app_p->app_init(gattc_if);
        break;

    case ESP_GATTC_OPEN_EVT:
        if (param->open.status) {
            esk8_log_E(ESK8_TAG_BLE,
                "Error openning gattc. Dev: %s " MACSTR ", App: %s, Status: %d\n",
                app_hndl->dev_p->name,
                MAC2STR(app_hndl->dev_p->addr),
                app_hndl->app_p->app_name,
                param->open.status
            );
            break;
        }
        app_hndl->state |= ESK8_BLEC_APP_HNDL_STATE_CONN;
        app_hndl->conn_id = param->open.conn_id;
        esp_ble_gattc_search_service(gattc_if, param->open.conn_id, NULL);
        break;

    case ESP_GATTC_CLOSE_EVT:
        app_hndl->state &= ~ESK8_BLEC_APP_HNDL_STATE_CONN;
        if (param->close.status)
            esk8_log_E(ESK8_TAG_BLE,
                "Got status %d closing conn to %s\n",
                param->close.status,
                app_hndl->dev_p->name
            );
        else
        {
            esk8_log_I(ESK8_TAG_BLE,
                "Closing conn to %s on app '%s'\n",
                app_hndl->dev_p->name,
                app_hndl->app_p->app_name
            );

            if (app_hndl->app_p->app_conn_del)
                app_hndl->app_p->app_conn_del(app_hndl->dev_p, param->close.conn_id);
        }
        break;

    case ESP_GATTC_SEARCH_RES_EVT:
        esk8_log_I(ESK8_TAG_BLE,
            "Found service. Start: %u, end: %u, is_primary: %d, srvc_uuid: 0x%04x\n",
            param->search_res.start_handle,
            param->search_res.end_handle,
            param->search_res.is_primary,
            param->search_res.srvc_id.uuid.uuid.uuid16
        );
        break;

    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (param->search_cmpl.status) {
            esk8_log_E(ESK8_TAG_BLE,
                "Got bad status: %d\n"
                , param->search_cmpl.status
            );
            break;
        }

        uint16_t attr_count = ESK8_CONFIG_BLEC_MAX_HNDL;
        esp_gattc_db_elem_t db[ESK8_CONFIG_BLEC_MAX_HNDL];
        esp_err_t err = esp_ble_gattc_get_db(
            gattc_if, param->search_cmpl.conn_id,
            0x0001, 0xFFFF, db, &attr_count
        );

        if (err) {
            esk8_log_I(ESK8_TAG_BLE,
                "Got %d attr. Err: %s\n",
                attr_count, esp_err_to_name(err)
            );
            break;
        }

        for (int i=0; i<attr_count; i++)
        {
            esk8_log_D(ESK8_TAG_BLE,
                "Got attr: 0x%04x, len: %d\n",
                db[i].uuid.uuid.uuid16,
                db[i].uuid.len
            );
        }

        if (attr_count != app_hndl->app_p->ble_elm_n) {
            esk8_log_E(ESK8_TAG_BLE,
                "Got wrong number of attributes: %d, expected %d. Disconnecting.\n",
                attr_count,
                app_hndl->app_p->ble_elm_n
            );

            esp_ble_gap_disconnect((uint8_t*)app_hndl->dev_p->addr);
            break;
        }

        for (int i=0; i < attr_count; i++) {
            esp_gattc_db_elem_t* dev_db = &db[i];
            esp_gattc_db_elem_t* clt_db = &app_hndl->app_p->ble_elm_l[i];

            if (dev_db->type != clt_db->type) {
                esk8_log_E(ESK8_TAG_BLE,
                    "Got type: %d, expected: %d. Index %d\n",
                    dev_db->type,
                    clt_db->type, i
                );
                goto attr_fail_break;
            }

            if (dev_db->uuid.len != clt_db->uuid.len) {
                esk8_log_E(ESK8_TAG_BLE,
                    "Got UUID len: %d, expected: %d. Index %d\n",
                    dev_db->uuid.len,
                    clt_db->uuid.len, i
                );
                goto attr_fail_break;
            }

            if (memcmp(&dev_db->uuid.uuid, &clt_db->uuid.uuid, clt_db->uuid.len) != 0) {
                esk8_log_E(ESK8_TAG_BLE,
                    "Got UUID: 0x%04x, expected: 0x%04x. Index %d\n",
                    dev_db->uuid.uuid.uuid16,
                    clt_db->uuid.uuid.uuid16, i
                );
                goto attr_fail_break;
            }
        }

        /* Success */
        esk8_log_I(ESK8_TAG_BLE,
            "Got valid dev: %s, passing to app: %s\n",
            app_hndl->dev_p->name,
            app_hndl->app_p->app_name
        );

        for (int i=0; i<attr_count; i++)
            app_hndl->app_p->ble_elm_l[i].attribute_handle = db[i].attribute_handle;

        if (app_hndl->app_p->app_conn_add)
            app_hndl->app_p->app_conn_add(
                app_hndl->dev_p,
                param->search_cmpl.conn_id
            );

attr_fail_break:
        break;

    default:
        break;
    }
}
