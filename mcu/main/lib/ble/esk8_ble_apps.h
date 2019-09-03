#ifndef _ESK8_BLE_APPS_H
#define _ESK8_BLE_APPS_H


typedef struct
{
    const char* name;
    void**      conn_param;

    void (*app_init)();
    void (*app_deinit)();
    void (*app_conn_add)(void** conn_new_param); // The pointer pointed by `conn_new_param` will be passed to `app_evt_cb`
    void (*app_conn_del)(void** conn_param);
    void (*app_evt_cb)(void* evt, void* conn_param);
}
esk8_ble_app_t;

extern esk8_ble_app_t* esk8_app_ble_list;



#endif /*_ESK8_BLE_APPS_H */
