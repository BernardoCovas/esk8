#ifndef _ESK8_BLEC_APPS_PRIV_H
#define _ESK8_BLEC_APPS_PRIV_H

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gattc_api.h>
#include <esp_gap_ble_api.h>

#include <stdint.h>

typedef
enum esk8_ble_app_stat
{
    ESK8_BLEC_STATE_NOINIT = 0,
    ESK8_BLEC_STATE_INIT,
    ESK8_BLEC_STATE_SEARCHING,
    ESK8_BLEC_STATE_CONNECTING,
    ESK8_BLEC_STATE_RUNNING,
}
esk8_blec_apps_state_t;

struct
{
    int gatt_if;
    esk8_blec_app_t* app;
    esk8_blec_conn_ctx_t* conn_ctx_list;
}
esk8_blec_app_hndl_t;

typedef struct
{
    uint n_dev;
    uint n_apps;
    uint n_conn;
    uint n_apps_max;
    uint n_conn_max;
    esk8_blec_dev_t**      dev_list;
    esk8_blec_app_t**      app_list;
    esk8_blec_conn_ctx_t*  conn_ctx_list;
    esk8_blec_apps_state_t state;
}
esk8_blec_apps_t;

extern esk8_blec_apps_t
    esk8_blec_apps;

void
esk8_blec_apps_gattc_cb(
    esp_gattc_cb_event_t event,
    esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t* param
);

void
esk8_blec_apps_gap_cb(
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param
);

esk8_blec_dev_t*
esk8_blec_apps_get_dev(
    uint8_t addr[6]
);

#endif /* _ESK8_BLEC_APPS_PRIV_H */
