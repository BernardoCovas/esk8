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
 * Function called for every event.
 */
typedef void(*e_ride_ble_evt_cb_t)(e_ride_ble_notif_t* evtNotif);


typedef struct
{
    uint16_t                desc_hndl;
    esp_bt_uuid_t           desc_uuid;
    esp_gatt_perm_t         desc_perm;
    esp_attr_value_t        desc_val;
    esp_attr_control_t      desc_ctrl;
} e_ride_ble_char_desc_t;


typedef struct
{
    uint16_t                char_hndl;
    esp_bt_uuid_t           char_uuid;
    esp_gatt_perm_t         char_perm;
    esp_gatt_char_prop_t    char_prop;
    esp_attr_control_t      char_ctrl;
    esp_attr_value_t        char_val;
    e_ride_ble_char_desc_t* char_desc_p;
} e_ride_ble_char_t;


/**
 * Function used to get the char by
 * char_handle.
 */
typedef e_ride_ble_char_t* (*e_ride_ble_app_get_char_t)(uint16_t char_handle);


typedef struct 
{
    const char*                 app_serviceName;
    uint16_t                    _app_appHndl;
    uint16_t                    _app_connId;

    esp_gatts_attr_db_t*        attr_list;
    uint16_t*                   attr_hndlList;
    uint8_t                     attr_numAttr;

    e_ride_ble_evt_cb_t         app_evtFunc;
    uint8_t                     app_srvcId;

}
e_ride_ble_app_t;


typedef struct
{
    void* dummy;
}
e_ride_ble_config_t;


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