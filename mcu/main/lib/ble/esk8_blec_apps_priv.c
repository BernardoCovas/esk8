#include <esk8_log.h>
#include <esk8_blec_apps.h>

#include "esk8_blec_apps_priv.h"

#include <string.h>


esk8_blec_conn_ctx_t*
esk8_blec_apps_get_ctx(
    uint gattc_if,
    uint conn_id
)
{
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

    if (event == ESP_GATTC_REG_EVT)
    {
        app = esk8_blec_apps.app_list[param->reg.app_id];
        esk8_blec_apps.app_ctx_list[param->reg.app_id].gattc_if = gattc_if;
    }
    else
        for (int i = 0; i < esk8_blec_apps.n_apps; i++)
        {
            if (esk8_blec_apps.app_ctx_list[i].gattc_if == gattc_if)
            {
                app = esk8_blec_apps.app_list[i];
                break;
            }
        }

    if (!app)
    {
        esk8_log_W(ESK8_TAG_BLE,
            "Got event %d with no associated app.\n", event
        );
        return;
    }

    switch (event)
    {
    case ESP_GATTC_CONNECT_EVT:
        // TODO
        break;
    
    default:
        break;
    }

    esk8_log_I(ESK8_TAG_BLE,
        "Got event %d, passing to app '%s'\n",
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

#define MAC_STR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_STR_PARAM(X) X[0], X[1], X[2], X[3], X[4], X[5], X[6]

    esk8_log_D(ESK8_TAG_BLE,
        "Got event: %d\n", event
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
            dev_name = (uint8_t*)"(no name)";
        else
            dev_name[ble_name_len] = 0;

        esk8_log_I(ESK8_TAG_BLE,
            "Found '%s': " MAC_STR "\n",
            dev_name,
            MAC_STR_PARAM(param->scan_rst.bda)
        );

        for (int i = 0; i < esk8_blec_apps.n_dev; i++)
        {
            esk8_blec_dev_t* dev = esk8_blec_apps.dev_list[i];
            if  (
                    memcmp(dev->addr, param->scan_rst.bda, 6) == 0 &&
                    strcmp(dev->name, dev_name) == 0
                )
            {
                esk8_log_I(ESK8_TAG_BLE,
                    "Connecting to %s\n",
                    dev_name
                );
            }
        }

        break;
    }
    
    default:
        break;
    }
}
