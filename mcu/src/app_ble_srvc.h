#ifndef _APP_BLE_SRVC
#define _APP_BLE_SRVC

#include <e_ride_ble.h>

extern e_ride_ble_app_t app_srvc_status;

#define APP_ALL_SRVC_LIST_P() { &app_srvc_status}


/**
 * Updates the internal speed value
 * that is used as response to
 * the speed characteristic.
 */
void app_srvc_status_update_speed(

    uint8_t     speedVal

);


/**
 * Updates the internal battery value
 * that is used as response to
 * the battery characteristic.
 */
void app_srvc_status_update_battery(

    uint8_t*    batVals,
    uint8_t     numBat

);


#endif /* _APP_BLE_SRVC */
