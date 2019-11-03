#ifndef _ESK8_BLE_DEFS_H
#define _ESK8_BLE_DEFS_H

#define ESK8_BLE_AUTH_UUID                  0xE8A0
#define ESK8_BLE_AUTH_KEY_UUID              0xE8A1
#define ESK8_BLE_AUTH_CHANGE_UUID           0xE8A2

#define ESK8_BLE_CTRL_UUID                  0xE8C0
#define ESK8_BLE_CTRL_SPEED_UUID            0xE8C1
#define ESK8_BLE_CTRL_PWR_UUID              0xE8C2

#define ESK8_BLE_STATUS_UUID                0xE8E0
#define ESK8_BLE_STATUS_SPEED_UUID          0xE8E1
#define ESK8_BLE_STATUS_BMS_SHALLOW_UUID    0xE8E2
#define ESK8_BLE_STATUS_BMS_DEEP_UUID       0xE8E3


typedef enum
{
    ESK8_BLE_DEV_CONNECT_FAILED = -1,

    ESK8_BLE_DEV_NOTFOUND,
    ESK8_BLE_DEV_FOUND,
    ESK8_BLE_DEV_CONNECTING,
    ESK8_BLE_DEV_CONNECTED,
}
esk8_ble_dev_state_t;

#endif /* _ESK8_BLE_DEFS_H */
