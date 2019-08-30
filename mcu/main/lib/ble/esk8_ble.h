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
}
esk8_ble_notif_t;


/**
 * Function called for every event.
 */
typedef void(*esk8_ble_evt_cb_t)(esk8_ble_notif_t* evtNotif);


typedef struct
{
    const char*                 app_serviceName;
    uint16_t                   _app_appHndl;
    uint16_t                   _app_connId;

    esp_gatts_attr_db_t*        attr_list;
    uint16_t*                   attr_hndlList;
    uint8_t                     attr_numAttr;

    esk8_ble_evt_cb_t           app_evtFunc;
    uint8_t                     app_srvcId;
}
esk8_ble_app_t;


typedef struct
{
    bool                        blck_conn;
}
esk8_ble_config_t;


typedef struct
{
    esk8_ble_config_t           bleCnfg;

    esk8_ble_app_t**            p_appList;
    uint16_t                    appNum;
}
esk8_ble_t;


/**
 *
 */
esk8_err_t
esk8_ble_init(
    esk8_ble_config_t*    bleCnfg);


/**
 * Free every resource associated
 * with the apps and ble manager.
 */
esk8_err_t
esk8_ble_close();


/**
 * Starts the ble apps.
 * Every app struct must exist
 * while the app is running.
 * Not the list of pointers,
 * but the apps.
 * Allocates resources.
 */
esk8_err_t
esk8_ble_register_apps(
    uint16_t            numApps,
    esk8_ble_app_t**  p_appList);


/**
 *
 */
esk8_err_t
esk8_ble_app_evnt_await(
    esk8_ble_app_handler_t appHndlr,
    esk8_ble_notif_t*      appNotif,
    uint32_t               timeout_ms);


/**
 *
 */
esk8_err_t
esk8_ble_get_idx_from_handle(
    esk8_ble_app_t* app,
    uint16_t        handle,
    int*            out_idx);


#endif /* _ESK8_BLE_H */
