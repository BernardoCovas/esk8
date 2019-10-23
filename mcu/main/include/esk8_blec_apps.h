#ifndef _ESK8_BLEC_APPS_H
#define _ESK8_BLEC_APPS_H

#include <esk8_err.h>
#include <esk8_ble_defs.h>

#include <esp_gattc_api.h>


typedef struct
{
    int     conn_id;
    void*   ctx;
}
esk8_blec_conn_ctx_t;

typedef struct
{
    const char*         app_name;
    esk8_ble_app_stat_t stat;

    esk8_err_t (*app_init      )();
    esk8_err_t (*app_deinit    )();
    esk8_err_t (*app_conn_add  )(esk8_blec_conn_ctx_t* conn_ctx);
    esk8_err_t (*app_conn_del  )(esk8_blec_conn_ctx_t* conn_ctx);
    esk8_err_t (*app_conn_notif)(esk8_blec_conn_ctx_t* conn_ctx, int attr_idx, size_t  len, uint8_t* val);
    esk8_err_t (*app_conn_read )(esk8_blec_conn_ctx_t* conn_ctx, int attr_idx, size_t  len, uint8_t* val);
    esk8_err_t (*app_evt_cb    )(esp_gattc_cb_event_t event, esp_ble_gattc_cb_param_t *param);
}
esk8_blec_app_t;

extern esk8_blec_app_t
    esk8_blec_app_ctrl;

esk8_err_t
esk8_ble_appc_init(
    uint n_apps_max
);

void
esk8_ble_appc_deinit(
);

esk8_err_t
esk8_ble_appc_app_reg(
    esk8_blec_app_t* app
);


#endif /* _ESK8_BLEC_APPS_H */
