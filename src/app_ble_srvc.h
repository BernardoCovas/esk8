#ifndef _APP_BLE_SRVC
#define _APP_BLE_SRVC

#include <e_ride_ble.h>

extern e_ride_ble_app_t app_srvc_status;
extern e_ride_ble_app_t app_srvc_ctrl;

#define APP_ALL_SRVC_LIST_P() { &app_srvc_status, &app_srvc_ctrl}

#endif /* _APP_BLE_SRVC */
