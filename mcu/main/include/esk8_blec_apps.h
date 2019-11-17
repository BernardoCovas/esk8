#ifndef _ESK8_BLEC_APPS_H
#define _ESK8_BLEC_APPS_H

#include <esk8_err.h>
#include <esk8_ble_defs.h>

#include <esp_gattc_api.h>

#ifndef ESK8_CONFIG_BLEC_SCAN_DURATION_s
#define ESK8_CONFIG_BLEC_SCAN_DURATION_s 10
#endif

#ifndef ESK8_CONFIG_BLEC_MAX_HNDL
#define ESK8_CONFIG_BLEC_MAX_HNDL 30
#endif

typedef struct
{
    const char*   name;
    const uint8_t addr[6];
}
esk8_blec_dev_t;

typedef struct
{
    const char* app_name;

    esk8_err_t (*app_init      )(int gattc_if);
    esk8_err_t (*app_deinit    )(int gattc_if);
    esk8_err_t (*app_conn_add  )(esk8_blec_dev_t* dev, uint16_t conn_id);
    esk8_err_t (*app_conn_del  )(esk8_blec_dev_t* dev, uint16_t conn_id);
    esk8_err_t (*app_conn_notif)(esk8_blec_dev_t* dev, int attr_idx, size_t len, uint8_t* val);
    esk8_err_t (*app_conn_read )(esk8_blec_dev_t* dev, int attr_idx, size_t  len, uint8_t* val);
    esk8_err_t (*app_evt_cb    )(esp_gattc_cb_event_t event, esp_ble_gattc_cb_param_t *param);

    uint16_t             ble_elm_n;
    esp_gattc_db_elem_t* ble_elm_l;
}
esk8_blec_app_t;

esk8_err_t
esk8_blec_init(
    uint16_t app_n_max
);

esk8_err_t
esk8_blec_deinit(
);

esk8_err_t
esk8_blec_app_reg(
    esk8_blec_app_t* app_p,
    esk8_blec_dev_t* dev_p
);

esk8_err_t
esk8_blec_search_start(
);

esk8_err_t
esk8_blec_search_stop(
);

esk8_err_t
esk8_blec_conn(
);

esk8_err_t
esk8_blec_close(
);

#endif /* _ESK8_BLEC_APPS_H */
