#ifndef _ESK8_BLE_APPC_PRIV_H
#define _ESK8_BLE_APPC_PRIV_H

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gattc_api.h>
#include <esp_gap_ble_api.h>

#include <stdint.h>


typedef struct
{
    uint16_t ble_if;
}
esk8_ble_appc_ctx_t;

typedef struct
{
    uint n_apps;
    uint n_apps_max;
    esk8_ble_appc_t** app_list;
    esk8_ble_appc_ctx_t* app_ctx_list;
}
esk8_ble_appc_hndl_t;

void
esk8_ble_appc_gattc_cb(
    esp_gattc_cb_event_t event,
    esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t* param
);

void
esk8_ble_appc_gap_cb(
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param
);


#endif /* _ESK8_BLE_APPC_PRIV_H */
