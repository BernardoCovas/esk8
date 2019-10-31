#ifndef _ESK8_BLES_APPS_H
#define _ESK8_BLES_APPS_H

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

    esk8_err_t (*app_init      )();
    esk8_err_t (*app_deinit    )();
    esk8_err_t (*app_conn_add  )(esk8_ble_conn_ctx_t* conn_ctx);
    esk8_err_t (*app_conn_del  )(esk8_ble_conn_ctx_t* conn_ctx);
    esk8_err_t (*app_conn_write)(esk8_ble_conn_ctx_t* conn_ctx, int attr_idx, size_t  len, uint8_t* val);
    esk8_err_t (*app_evt_cb    )(esp_gatts_cb_event_t event, esp_ble_gatts_cb_param_t *param);

    esp_gatts_attr_db_t*    attr_db;
    uint16_t                attr_num;

    esk8_ble_conn_ctx_t*    _conn_ctx_list;
    esp_gatts_attr_db_t*    _attr_list;
    uint16_t*               _attr_hndl_list;
    uint16_t                _ble_if;
}
esk8_bles_app_t;

typedef struct
{
    esk8_bles_app_t** apps_list;
    uint apps_num_max;
    uint conn_num_max;
    uint curr_app_id;
}
esk8_bles_apps_t;

extern esk8_bles_apps_t
    esk8_bles_apps;

extern esk8_bles_app_t
    esk8_app_srvc_auth,
    esk8_app_srvc_status,
    esk8_app_srvc_ctrl;

esk8_err_t
esk8_bles_apps_init(
    uint n_apps_max,
    uint n_conn_max
);

esk8_err_t
esk8_bles_app_register(
    esk8_bles_app_t* app
);

esk8_err_t
esk8_bles_apps_deinit(
);


#endif /*_ESK8_BLES_APPS_H */
