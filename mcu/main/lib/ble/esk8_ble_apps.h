#ifndef _ESK8_BLE_APPS_H
#define _ESK8_BLE_APPS_H

#include <esk8_err.h>

#include <esp_gatts_api.h>

#include <stdint.h>


typedef struct
{
    int     conn_id;
    void*   ctx;
}
esk8_ble_conn_ctx_t;

typedef struct
{
    const char* app_name;

    void (*app_init      )();
    void (*app_deinit    )();
    void (*app_conn_add  )(esk8_ble_conn_ctx_t* conn_ctx);
    void (*app_conn_del  )(esk8_ble_conn_ctx_t* conn_ctx);
    void (*app_conn_write)(esk8_ble_conn_ctx_t* conn_ctx, int attr_idx, size_t  len, uint8_t* val);
    void (*app_evt_cb    )(esp_gatts_cb_event_t event, esp_ble_gatts_cb_param_t *param);


    esp_gatts_attr_db_t*    attr_db;
    uint16_t                attr_num;

    esk8_ble_conn_ctx_t*    _conn_ctx_list;
    esp_gatts_attr_db_t*    _attr_list;
    uint16_t*               _attr_hndl_list;
    uint16_t                _ble_if;
}
esk8_ble_app_t;

typedef struct
{
    esk8_ble_app_t* apps_list;
    unsigned int    apps_num_max;
    unsigned int    conn_num_max;
    unsigned int    curr_app_id;
}
esk8_ble_apps_t;

extern esk8_ble_apps_t esk8_ble_apps;
extern esk8_ble_app_t  esk8_app_srvc_auth;
extern esk8_ble_app_t  esk8_app_srvc_status;
extern esk8_ble_app_t  esk8_app_srvc_ctrl;

/**
 *
 */
esk8_err_t
esk8_ble_apps_init(
    unsigned int    n_apps_max,
    unsigned int    n_conn_max
);

/**
 *
 */
esk8_err_t
esk8_ble_app_register(
    esk8_ble_app_t* app
);


/**
 *
 */
esk8_err_t
esk8_ble_apps_deinit();


#endif /*_ESK8_BLE_APPS_H */
