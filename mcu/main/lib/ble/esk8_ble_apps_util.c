#include "esk8_ble_apps.h"
#include "esk8_ble_apps_util.h"

#include <esk8_err.h>
#include <esk8_log.h>


esk8_err_t
esk8_ble_apps_get_ctx(
    esk8_ble_app_t*       app,
    uint16_t              conn_id,
    esk8_ble_conn_ctx_t** out_ctx_p
)
{
    for (int i = 0; i < esk8_ble_apps.conn_num_max; i++)
        if (app->_conn_ctx_list[i].conn_id == conn_id)
        {
            (*out_ctx_p) = &app->_conn_ctx_list[i];
            return ESK8_OK;
        }

    (*out_ctx_p) = NULL;
    return ESK8_ERR_INVALID_PARAM;
}

esk8_err_t
esk8_ble_apps_get_attr_idx(
    esk8_ble_app_t* app,
    uint16_t        handle,
    int*            out_idx
)
{
    for (int i=0; i<app->attr_num; i++)
        if (app->_attr_hndl_list[i] == handle)
        {
            (*out_idx) = i;
            return ESK8_OK;
        }

    return ESK8_ERR_INVALID_PARAM;
}

esk8_err_t
esk8_ble_apps_update(
    esk8_ble_app_t* app,
    int             attr_idx,
    size_t          val_len,
    uint8_t*        val
)
{
    if (attr_idx > app->attr_num)
        return ESK8_ERR_INVALID_PARAM;

    if (!app->_conn_ctx_list)
        return ESK8_BLE_APP_NOREG;

    esp_ble_gatts_set_attr_value(
        app->_attr_hndl_list[attr_idx],
        val_len, val
    );

    return ESK8_OK;
}

esk8_err_t
esk8_ble_apps_notify_all(
    esk8_ble_app_t* app,
    int             attr_idx,
    size_t          val_len,
    uint8_t*        val
)
{
    for (int i = 0; i < esk8_ble_apps.conn_num_max; i++)
    {
        int conn_id = app->_conn_ctx_list[i].conn_id;
        if (conn_id < 0)
            continue;

        printf(ESK8_TAG_BLE "Notifying conn id %d, from '%s'\n",
            conn_id, app->app_name);

        esp_ble_gatts_send_indicate(
            app->_ble_if,
            conn_id,
            app->_attr_hndl_list[attr_idx],
            val_len,
            val, false
        );
    }

    return ESK8_OK;
}
