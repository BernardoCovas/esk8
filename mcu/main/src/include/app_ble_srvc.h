#ifndef _APP_BLE_SRVC
#define _APP_BLE_SRVC

#include <esk8_ble.h>
#include <esk8_bms.h>


extern uint16_t SRVC_STATUS_UUID;
extern uint16_t SRVC_STATUS_SPEED_UUID;
extern uint16_t SRVC_STATUS_BMS_SHALLOW_UUID;
extern uint16_t SRVC_STATUS_BMS_DEEP_UUID;

extern uint16_t SRVC_CTRL_UUID;
extern uint16_t SRVC_CTRL_SPEED_UUID;
extern uint16_t SRVC_CTRL_PWR_UUID;

extern uint16_t SRVC_AUTH_UUID;
extern uint16_t SRVC_AUTH_KEY_UUID;
extern uint16_t SRVC_AUTH_CHANGE_UUID;

extern esk8_ble_app_t app_srvc_status;
extern esk8_ble_app_t app_srvc_ctrl;
extern esk8_ble_app_t app_srvc_auth;

#define APP_ALL_SRVC_LIST_P() { &app_srvc_status, &app_srvc_ctrl, &app_srvc_auth }


/**
 * Updates the internal speed value
 * that is used as response to
 * the speed characteristic.
 */
void app_srvc_status_update_speed(

    uint8_t     speedVal

);


/**
 * Updates the internal status
 * that is used as response to
 * the shallow bms status characteristic.
 * Also, if a connection is active
 * with notify enabled, this will
 * send a notification with the error code
 * so the client can pull the data.
 */
esk8_err_t app_srvc_status_update_bms_shallow(

    esk8_err_t            bmsErrCode,
    uint16_t                bmsIndex,
    esk8_bms_status_t*    bmsStatus

);


/**
 * Updates the internal status
 * that is used as response to
 * the deep bms status characteristic.
 * Also, if a connection is active
 * with notify enabled, this will
 * send a notification with the error code
 * so the client can pull the data.
 */
esk8_err_t app_srvc_status_update_bms_deep(

    esk8_err_t                bmsErrCode,
    uint16_t                    bmsIndex,
    esk8_bms_deep_status_t*   bmsStatus

);


#endif /* _APP_BLE_SRVC */
