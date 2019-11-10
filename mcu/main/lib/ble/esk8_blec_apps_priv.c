#include <esk8_log.h>
#include <esk8_blec_apps.h>
#include "esk8_blec_apps_priv.h"

#include <esp_log.h>


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
                "Found: " MACSTR "\n",
                MAC2STR(param->scan_rst.bda)
            );
        } else
        {
            dev_name[dev_name_len] = 0;
            esk8_log_I(ESK8_TAG_BLE,
                "Found: %s " MACSTR "\n",
                dev_name,
                MAC2STR(param->scan_rst.bda)
            );
        }
    }
        break;

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

}
