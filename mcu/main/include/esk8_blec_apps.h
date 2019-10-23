#ifndef _ESK8_BLEC_APPS_H
#define _ESK8_BLEC_APPS_H

#include <esk8_err.h>

#include <esp_gattc_api.h>


typedef struct
{
    const char* app_name;

    void (*app_init      )();
    void (*app_deinit    )();

    void (*app_evt_cb    )(esp_gattc_cb_event_t event, esp_ble_gattc_cb_param_t *param);
}
esk8_ble_appc_t;

esk8_err_t
esk8_ble_appc_init(
    uint n_apps_max
);

void
esk8_ble_appc_deinit(
);

esk8_err_t
esk8_ble_appc_app_reg(
    esk8_ble_appc_t* app
);


#endif /* _ESK8_BLEC_APPS_H */
