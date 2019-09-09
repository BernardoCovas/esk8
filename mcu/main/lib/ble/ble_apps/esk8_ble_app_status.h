#ifndef _ESK8_BLE_APP_STATUS_H
#define _ESK8_BLE_APP_STATUS_H

#include <esk8_err.h>


esk8_err_t
esk8_ble_app_status_speed(
    uint8_t speed_val
);

esk8_err_t
esk8_ble_app_status_bms_shallow(
    esk8_bms_status_t* stat,
    esk8_err_t         bms_err_code,
    int                bms_idx
);

esk8_err_t
esk8_ble_app_status_bms_deep(
    esk8_bms_deep_status_t* stat,
    esk8_err_t              bms_err_code,
    int                     bms_idx
);

#endif /* _ESK8_BLE_APP_STATUS_H */