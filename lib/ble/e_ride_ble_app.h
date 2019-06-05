#ifndef _E_RIDE_BLE_APP_H
#define _E_RIDE_BLE_APP_H

#include <e_ride_err.h>
#include <e_ride_ble.h>

#include <stdint.h>


/**
 * Handler representing one app.
 */
typedef uint16_t e_ride_ble_app_handler_t;


/**
 * 
 */
typedef enum
{
    E_RIDE_BLE_PERM_READ     = 1 << 0,
    E_RIDE_BLE_PERM_WRITE    = 1 << 1,
} e_ride_ble_perm_t;


/**
 * 
 */
typedef struct
{
    const char*         app_displayName;

    uint8_t             app_srvcUuid[32];
    e_ride_ble_perm_t     app_srvcPerm;

    uint8_t             app_numChar;
    uint16_t*            app_charUuid;
    e_ride_ble_perm_t*    app_charPerm;

    uint32_t            _initFlag;    
    void*                _evntQueue;

} e_ride_ble_app_t;


/**
 *
 */
e_ride_err_t e_ride_ble_app_register(

    e_ride_ble_app_t        * app,
    e_ride_ble_app_handler_t* appHndlr

);


/**
 * 
 */
e_ride_err_t e_ride_ble_app_evnt_await(

    e_ride_ble_app_handler_t appHndlr

);


#endif /* _E_RIDE_BLE_APP_H */