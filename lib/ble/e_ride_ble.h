#ifndef _E_RIDE_BLE_H
#define _E_RIDE_BLE_H

#include <e_ride_err.h>

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


typedef enum
{
    E_RIDE_BLE_EVT_REGISTD,
    E_RIDE_BLE_EVT_NEWCONN,
    E_RIDE_BLE_EVT_DISCONN,
} e_ride_ble_event_t;


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


typedef union
{
    struct
    {
        e_ride_ble_app_handler_t appHandler;
    } reg;

    struct
    {
        uint8_t mac[6];
        uint32_t devHandler;
    } connect;

    struct
    {
        uint32_t devHandler;
    } disconnect;

    struct
    {
        uint32_t charHandle;
    } readChar;


    struct
    {
        uint32_t charHandle;
    } writeChar;

} e_ride_ble_param_t;


typedef struct
{
    e_ride_ble_event_t event;
    e_ride_ble_param_t param;
} e_ride_ble_notif_t;


typedef struct
{
    size_t evtQueueLen;
    /* TODO */
} e_ride_ble_config_t;


typedef struct
{
    e_ride_ble_config_t bleCnfg;

    e_ride_ble_app_t**  p_appList;
    uint16_t            appNum;
} e_ride_ble_t;


/**
 *
 */
e_ride_err_t e_ride_ble_init(

    e_ride_ble_config_t*    bleCnfg

);


/**
 * Free every resource associated
 * with the app and ble manager.
 */
e_ride_err_t e_ride_ble_close();

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
    e_ride_ble_notif_t *     appNotif,
    uint32_t                 timeout_ms

);


#endif /* _E_RIDE_BLE_H */
