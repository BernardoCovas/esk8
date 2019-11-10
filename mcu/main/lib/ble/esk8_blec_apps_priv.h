#ifndef ESK8_BLEC_APPS_PRIV_H
#define ESK8_BLEC_APPS_PRIV_H

#include <esk8_blec_apps.h>

#include <esp_gap_ble_api.h>
#include <esp_gattc_api.h>

enum
{
    ESK8_BLEC_APP_HNDL_STATE_INIT       = (1 << 0), /* Init func was called */
    ESK8_BLEC_APP_HNDL_STATE_DEV_FOUND  = (1 << 1), /* Device was found while searching */
    ESK8_BLEC_APP_HNDL_STATE_CONN       = (1 << 2), /* Connected to device */
    ESK8_BLEC_APP_HNDL_STATE_BROKEN     = (1 << 3), /* If the device disconnected after being connected (pipe broken), resets if reconnected  */
};

enum
{
    ESK8_BLEC_APPS_STATE_INIT       = (1 << 0),
    ESK8_BLEC_APPS_STATE_SEARCHING  = (1 << 1),
    ESK8_BLEC_APPS_STATE_CONNECTING = (1 << 2),
};

typedef struct
{
    uint gattc_if;
    uint16_t state;
    esk8_blec_app_t* app_p;
    esk8_blec_dev_t* dev_p;
}
esk8_blec_app_hndl_t;

typedef struct
{
    uint16_t state;
    uint16_t app_n;
    uint16_t app_n_max;
    esk8_blec_app_hndl_t* app_hndl_l;
}
esk8_blec_apps_t;

extern esk8_blec_apps_t
esk8_blec_apps;

void
esk8_blec_apps_gap_cb(
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param
);

void esk8_blec_apps_gattc_cb(
    esp_gattc_cb_event_t event,
    esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t *param
);

#endif /* ESK8_BLEC_APPS_PRIV_H */
