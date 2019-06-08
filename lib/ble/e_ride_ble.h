#ifndef _E_RIDE_BLE_H
#define _E_RIDE_BLE_H

#include <e_ride_err.h>
#include <e_ride_ble_app.h>

#include <stddef.h>


typedef enum
{
    E_RIDE_BLE_EVT_REGISTD,
    E_RIDE_BLE_EVT_NEWCONN,
    E_RIDE_BLE_EVT_DISCONN,
} e_ride_ble_event_t;


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


typedef (void*)(e_ride_ble_notif_hndlr_t)(e_ride_ble_notif_t notif);


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


#endif /* _E_RIDE_BLE_H */
