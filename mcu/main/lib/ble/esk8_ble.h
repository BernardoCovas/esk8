#ifndef _ESK8_BLE_H
#define _ESK8_BLE_H

#include <esk8_err.h>

#include <esp_gatts_api.h>

#include <stdint.h>
#include <stddef.h>


/**
 * Handler representing one app.
 */
typedef uint16_t esk8_ble_app_handler_t;


typedef struct
{
    esp_gatts_cb_event_t        event;
    esp_ble_gatts_cb_param_t*   param;
} esk8_ble_notif_t;


/**
 * Function called for every event.
 */
typedef void(*esk8_ble_evt_cb_t)(esk8_ble_notif_t* evtNotif);


typedef struct
{
    uint16_t                desc_hndl;
    esp_bt_uuid_t           desc_uuid;
    esp_gatt_perm_t         desc_perm;
    esp_attr_value_t        desc_val;
    esp_attr_control_t      desc_ctrl;
} esk8_ble_char_desc_t;


typedef struct
{
    uint16_t                char_hndl;
    esp_bt_uuid_t           char_uuid;
    esp_gatt_perm_t         char_perm;
    esp_gatt_char_prop_t    char_prop;
    esp_attr_control_t      char_ctrl;
    esp_attr_value_t        char_val;
    esk8_ble_char_desc_t* char_desc_p;
} esk8_ble_char_t;


/**
 * Function used to get the char by
 * char_handle.
 */
typedef esk8_ble_char_t* (*esk8_ble_app_get_char_t)(uint16_t char_handle);


typedef struct 
{
    const char*                 app_serviceName;
    uint16_t                    _app_appHndl;
    uint16_t                    _app_connId;

    esp_gatts_attr_db_t*        attr_list;
    uint16_t*                   attr_hndlList;
    uint8_t                     attr_numAttr;

    esk8_ble_evt_cb_t         app_evtFunc;
    uint8_t                     app_srvcId;
}
esk8_ble_app_t;


typedef struct
{
    bool  blck_conn;
}
esk8_ble_config_t;


typedef struct
{
    esk8_ble_config_t bleCnfg;

    esk8_ble_app_t**  p_appList;
    uint16_t            appNum;
}
esk8_ble_t;


/**
 *
 */
esk8_err_t esk8_ble_init(

    esk8_ble_config_t*    bleCnfg

);


/**
 * Free every resource associated
 * with the apps and ble manager.
 */
esk8_err_t esk8_ble_close();

/**
 * Starts the ble apps.
 * Every app struct must exist
 * while the app is running.
 * Not the list of pointers,
 * but the apps.
 * Allocates resources.
 */
esk8_err_t esk8_ble_register_apps(

    uint16_t            numApps,
    esk8_ble_app_t**  p_appList

);


/**
 *
 */
esk8_err_t esk8_ble_app_evnt_await(

    esk8_ble_app_handler_t appHndlr,
    esk8_ble_notif_t*      appNotif,
    uint32_t                 timeout_ms

);


/**
 * If the config is set to restrict
 * connections, this will allow them
 * temporarily. If a connection is made
 * during this time, the connection address
 * is saved and only connection requests
 * from this address are allowed.
 *
 * `timeout_ms`: If no connections are
 * attempted in this ammount of time,
 * cancel and relock.
 */
esk8_err_t esk8_ble_conn_allow(

    uint32_t timeout_ms

);


/**
 * If the config is set to restrict
 * connections and the system is
 * currently allowing them, this
 * will cancel and relock.
 */
esk8_err_t esk8_ble_conn_block();


#endif /* _ESK8_BLE_H */
