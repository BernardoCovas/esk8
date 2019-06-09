#ifndef _E_RIDE_BLE_H
#define _E_RIDE_BLE_H

#include <e_ride_err.h>

#include <esp_gatts_api.h>

#include <stdint.h>
#include <stddef.h>


/**
 * Handler representing one app.
 */
typedef uint16_t e_ride_ble_app_handler_t;


typedef struct
{
    esp_gatts_cb_event_t        event;
    esp_ble_gatts_cb_param_t*   param;
} e_ride_ble_notif_t;


/**
 * Function called for every enent.
 */
typedef void(*e_ride_ble_evt_cb_t)(e_ride_ble_notif_t* evtNotif);


typedef struct
{
    uint16_t                char_hndl;
    esp_bt_uuid_t           char_uuid;
    esp_gatt_perm_t         char_perm;
    esp_gatt_char_prop_t    char_prop;
    esp_attr_control_t      char_ctrl;
    esp_attr_value_t        char_val;
} e_ride_ble_char_t;

typedef struct
{
    uint16_t        svc_hndl;
    esp_gatt_id_t   srv_gattId;
} e_ride_ble_service_t;


typedef struct
{
    char*                 app_serviceName;
    esp_gatt_srvc_id_t    app_serviceId;

    uint8_t               app_numChar;
    e_ride_ble_char_t**   app_charList_p;
    e_ride_ble_evt_cb_t   app_evtFunc;
    uint16_t                    app_appHndl;
} e_ride_ble_app_t;


typedef struct
{
    /* TODO */
} e_ride_ble_config_t;


typedef struct
{
    e_ride_ble_config_t bleCnfg;

    e_ride_ble_app_t**  p_appList;
    uint16_t            appNum;
    void*               charSmph;
} e_ride_ble_t;


/**
 *
 */
e_ride_err_t e_ride_ble_init(

    e_ride_ble_config_t*    bleCnfg

);


/**
 * Free every resource associated
 * with the apps and ble manager.
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
