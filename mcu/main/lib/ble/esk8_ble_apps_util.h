#ifndef _ESK8_BLE_APPS_UTIL_H
#define _ESK8_BLE_APPS_UTIL_H

#include "esk8_ble_apps.h"
#include <esk8_err.h>


esk8_err_t
esk8_ble_apps_get_ctx(
    esk8_ble_app_t*       app,
    uint16_t              conn_id,
    esk8_ble_conn_ctx_t** out_ctx_p
);

esk8_err_t
esk8_ble_apps_get_attr_idx(
    esk8_ble_app_t* app,
    uint16_t        handle,
    int*            out_idx
);

esk8_err_t
esk8_ble_apps_notify_all(
    esk8_ble_app_t* app,
    int             attr_idx,
    size_t          val_len,
    uint8_t*        val
);

#endif /* _ESK8_BLE_APPS_UTIL_H */
