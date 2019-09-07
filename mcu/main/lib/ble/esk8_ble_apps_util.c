#include "esk8_ble_apps.h"
#include "esk8_ble_apps_util.h"

#include <esk8_err.h>


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
            return ESK8_SUCCESS;
        }

    (*out_ctx_p) = NULL;
    return ESK8_ERR_INVALID_PARAM;
}

esk8_err_t esk8_ble_apps_get_attr_idx(
    esk8_ble_app_t* app,
    uint16_t        handle,
    int*            out_idx
)
{
    for (int i=0; i<app->attr_num; i++)
        if (app->_attr_hndl_list[i] == handle)
        {
            (*out_idx) = i;
            return ESK8_SUCCESS;
        }

    return ESK8_ERR_INVALID_PARAM;
}
