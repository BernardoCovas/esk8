#ifndef _ESK8_BLEC_APPS_H
#define _ESK8_BLEC_APPS_H

#include <esk8_err.h>
#include <esk8_ble_defs.h>

#include <esp_gattc_api.h>


typedef struct
{
    const char*   name;
    const uint8_t addr[6];
    esk8_ble_dev_state_t state;
}
esk8_blec_dev_t;

typedef struct
{
    void* app_conn_ctx; /* Can be allocated and freed by the app */
}
esk8_blec_conn_ctx_t;

typedef struct
{
    const char* app_name;

    esk8_err_t (*app_init      )(int gatt_if);
    esk8_err_t (*app_deinit    )();
    esk8_err_t (*app_conn_add  )(void** conn_ctx_p);
    esk8_err_t (*app_conn_del  )(void** conn_ctx_p);
    esk8_err_t (*app_conn_notif)(void** conn_ctx_p, int attr_idx, size_t  len, uint8_t* val);
    esk8_err_t (*app_conn_read )(void** conn_ctx_p, int attr_idx, size_t  len, uint8_t* val);
    esk8_err_t (*app_evt_cb    )(esp_gattc_cb_event_t event, esp_ble_gattc_cb_param_t *param);
}
esk8_blec_app_t;

extern esk8_blec_app_t
    esk8_blec_app_ctrl;

esk8_err_t
esk8_blec_apps_init(
    uint n_apps_max,
    uint n_conn_max
);

void
esk8_blec_apps_deinit(
);

esk8_err_t
esk8_blec_apps_app_reg(
    esk8_blec_app_t* app
);

esk8_err_t
esk8_blec_apps_dev_reg(
    esk8_blec_dev_t* dev
);

esk8_err_t
esk8_blec_search_start(
);

esk8_err_t
esk8_blec_search_stop(
);


/**
 * Close conn
 */
esk8_err_t
esk8_blec_dscn(
);

#endif /* _ESK8_BLEC_APPS_H */
