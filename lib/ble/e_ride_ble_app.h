#ifndef _E_RIDE_BLE_APP_H
#define _E_RIDE_BLE_APP_H

#include <e_ride_err.h>
#include <e_ride_ble.h>

#include <stdint.h>
#include <stddef.h>


/**
 * Handler representing one app.
 */
typedef uint16_t e_ride_ble_app_handler_t;


/**
 *
 */
typedef enum
{
    E_RIDE_BLE_PERM_READ     = (1 << 0),
    E_RIDE_BLE_PERM_WRITE    = (1 << 1),
} e_ride_ble_perm_t;


/**
 *
 */
typedef struct
{
    const char *                app_displayName;

    const uint8_t               app_srvcUuid[32];
    const e_ride_ble_perm_t     app_srvcPerm;

    const uint8_t               app_numChar;
    const uint16_t*             app_charUuid;
    const e_ride_ble_perm_t*    app_charPerm;

    uint32_t                    _appHandlr;
    void*                       _evntQueue;
} e_ride_ble_app_t;


/**
 * Starts the ble apps.
 * Every app struct must exist
 * while the app is running.
 * Not the list of pointers,
 * but the apps.
 * Allocates resources.
 */
e_ride_err_t e_ride_ble_register_apps(

    uint16_t            numApps,
    e_ride_ble_app_t**  p_appList

);


/**
 *
 */
e_ride_err_t e_ride_ble_app_evnt_await(

    e_ride_ble_app_handler_t appHndlr,
    e_ride_ble_notif_t*      appNotif,
    uint32_t                 timeout_ms

);


#endif /* _E_RIDE_BLE_APP_H */