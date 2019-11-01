#ifndef _ESK8_BLES_APPS_PRIV_H
#define _ESK8_BLES_APPS_PRIV_H

#include <esk8_bles_apps.h>
#include <esk8_err.h>


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

esk8_err_t
esk8_bles_apps_get_ctx(
    esk8_bles_app_t*       app,
    uint16_t              conn_id,
    esk8_bles_conn_ctx_t** out_ctx_p
);

esk8_err_t
esk8_bles_apps_get_attr_idx(
    esk8_bles_app_t* app,
    uint16_t        handle,
    int*            out_idx
);

esk8_err_t
esk8_bles_apps_update(
    esk8_bles_app_t* app,
    int             attr_idx,
    size_t          val_len,
    uint8_t*        val
);

esk8_err_t
esk8_bles_apps_notify_all(
    esk8_bles_app_t* app,
    int             attr_idx,
    size_t          val_len,
    uint8_t*        val
);

#endif /* _ESK8_BLES_APPS_PRIV_H */
