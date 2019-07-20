#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_ble.h>
#include <esk8_nvs.h>

#include <esp_log.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>
#include <nvs_flash.h>

#include <string.h>
#include <stddef.h>


static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
//  .peer_addr          = {0},
//  .peer_addr_type     = 0,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};


static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = ESP_BLE_APPEARANCE_GENERIC_PERSONAL_MOBILITY_DEVICE, /* Oh yes */
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 0,
    .p_service_uuid = NULL,
    .flag = 0,
};


/**
 * Global BLE allocation.
 */
static esk8_ble_t bleHandler = {0};


esk8_err_t esk8_ble_conn_clear();


void esk8_gatts_event_hndlr(

    esp_gatts_cb_event_t event,
    esp_gatt_if_t gatts_if,
    esp_ble_gatts_cb_param_t *param

);


void esk8_gap_event_hndlr(

    esp_gap_ble_cb_event_t  event,
    esp_ble_gap_cb_param_t* param

);


esk8_ble_app_t* esk8_ble_get_app_from_if(

    uint16_t gatts_if

)
{
    for (int i=0; i<bleHandler.appNum; i++)
        if (bleHandler.p_appList[i]->_app_appHndl == gatts_if)
            return bleHandler.p_appList[i];

    return NULL;
}


esk8_err_t esk8_ble_init(

    esk8_ble_config_t*    bleCnfg

)
{
     /**
     * NOTE: (b.covas) NVS Has to be initialized
     * for BLE to work. If NVS was already initialized,
     * nvs_flash_init() will still return OK.
     */
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    esp_bt_controller_config_t bt_Cnfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(    ret                                                         );
    ESP_ERROR_CHECK(    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT)       );
    ESP_ERROR_CHECK(    esp_bt_controller_init(&bt_Cnfg)                            );
    ESP_ERROR_CHECK(    esp_bt_controller_enable(ESP_BT_MODE_BLE)                   );

    ESP_ERROR_CHECK(    esp_bluedroid_init()                                        );
    ESP_ERROR_CHECK(    esp_bluedroid_enable()                                      );

    ESP_ERROR_CHECK(    esp_ble_gap_register_callback(esk8_gap_event_hndlr)         );
    ESP_ERROR_CHECK(    esp_ble_gap_set_device_name(ESK8_BLE_DEV_NAME)              );
    ESP_ERROR_CHECK(    esp_ble_gatts_register_callback(esk8_gatts_event_hndlr)     );
    ESP_ERROR_CHECK(    esp_ble_gap_config_adv_data(&adv_data)                      );

    esk8_err_t err_code;
    esp_err_t _err_code;

    ESK8_ERRCHECK_THROW(esk8_ble_conn_clear());

    err_code = esk8_nvs_init();
    if (err_code == ESK8_NVS_NO_SETTINGS) return ESK8_SUCCESS;
    ESK8_ERRCHECK_THROW(err_code);

    esk8_nvs_settings_t sttgs;
    ESK8_ERRCHECK_THROW(esk8_nvs_settings_get(&sttgs));

    _err_code = esp_ble_gap_update_whitelist(
        true, sttgs.esk8_ble_peer_addr, BLE_WL_ADDR_TYPE_RANDOM);

    if (_err_code)
        return ESK8_BLE_FAILED_WL;

    return ESK8_SUCCESS;
}


esk8_err_t esk8_ble_close()
{
    if (!bleHandler.p_appList)
        return ESK8_SUCCESS;

    for (uint16_t i=0; i<bleHandler.appNum; i++)
        esp_ble_gatts_app_unregister(i);

    free(bleHandler.p_appList);

    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();

    return ESK8_SUCCESS;
}


esk8_err_t esk8_ble_register_apps(

    uint16_t           numApps,
    esk8_ble_app_t** p_appList

)
{
    static size_t currAppId = 0;

    if (!numApps || !p_appList)
        return ESK8_ERR_INVALID_PARAM;

    if (currAppId > 0)
        /**
         * This function was already called.
         * We don't want that. It can only
         * be called once.
         */
        return ESK8_BLE_INIT_REINIT;

    /**
     * Allocate app list.
     */
    bleHandler.p_appList = calloc(numApps, sizeof(void*));
    if (!bleHandler.p_appList)
        return ESK8_ERR_OOM;

    bleHandler.appNum = numApps;

    /**
     * Copy all the app's addresses to the bleHandler.
     * They are assumed to last for the time ble is
     * running.
     */
    memcpy((void*)bleHandler.p_appList, (void*)p_appList, sizeof(esk8_ble_app_t*) * numApps);

    /**
     * Register the apps with ble.
     */
    for (size_t i = 0; i<numApps; i++)
        ESP_ERROR_CHECK(esp_ble_gatts_app_register(currAppId++));

    return ESK8_SUCCESS;
}


void esk8_gatts_event_hndlr(

    esp_gatts_cb_event_t event,
    esp_gatt_if_t gatts_if,
    esp_ble_gatts_cb_param_t* param

)
{
    esk8_ble_app_t*  bleApp = esk8_ble_get_app_from_if(gatts_if);
    esk8_ble_notif_t bleNotif = {
        .event = event,
        .param = param
    };

    switch (event)
    {
        /**
         * Register an app.
         */
        case ESP_GATTS_REG_EVT:
        {
            uint16_t appId = param->reg.app_id;

            /**
             * The app id is the index.
             * Check if some some reason
             * the index is unexpected.
             */
            if (appId > bleHandler.appNum)
            {
                ESP_LOGE("APP_REGISTER", "App id was %d, but there are %d apps.", appId, bleHandler.appNum);
                return;
            }

            /**
             * The handler is the interface associated
             * with the app.
             */
            bleApp = bleHandler.p_appList[appId];
            bleApp->_app_appHndl = gatts_if;

            ESP_ERROR_CHECK(esp_ble_gatts_create_attr_tab(
                bleApp->attr_list,
                gatts_if,
                bleApp->attr_numAttr,
                bleApp->app_srvcId
            ));

            break;
        }

        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        {
            if (param->add_attr_tab.status != ESP_OK)
            {
                ESP_LOGE("CREATE_TABLE", "Error creating table: %d", param->add_attr_tab.status);
                break;
            }

            if (!bleApp)
            {
                ESP_LOGE("CREATE_TABLE", "Table created but no app associated with gatts if: %d", gatts_if);
                break;
            }

            if (param->add_attr_tab.num_handle != bleApp->attr_numAttr)
            {
                ESP_LOGE("CREATE_TABLE",
                    "Table created but the returned number of handles is %d. Expected %d instead.",
                    param->add_attr_tab.num_handle,
                    bleApp->attr_numAttr
                );
                break;
            }

            memcpy(bleApp->attr_hndlList, param->add_attr_tab.handles, bleApp->attr_numAttr * sizeof(uint16_t));

            esp_ble_gatts_start_service(bleApp->attr_hndlList[0]); /* Service is idx 0 */
            break;
        }

        case ESP_GATTS_DISCONNECT_EVT:
            ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&adv_params));
            break;

        case ESP_GATTS_CONNECT_EVT:

            if (bleApp)
                bleApp->_app_connId = param->connect.conn_id;

        {
            esp_ble_gap_update_whitelist(true, param->connect.remote_bda, BLE_WL_ADDR_TYPE_RANDOM);
            adv_params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST;
            esp_ble_gap_start_advertising(&adv_params);

            /* Update settings */
            esk8_nvs_settings_t sttgs;
            esk8_err_t err_code;
            err_code = esk8_nvs_settings_get(&sttgs);
            if (err_code)
                break;

            memcpy(sttgs.esk8_ble_peer_addr, param->connect.remote_bda, sizeof(sttgs.esk8_ble_peer_addr));
            esk8_nvs_settings_set(&sttgs);
        }
            break;
        default:
            break;
    }

    /**
     * Find which app this event is meant to,
     * and jump to it's event function.
     */
    bleApp = esk8_ble_get_app_from_if(gatts_if);
    if (bleApp && bleApp->app_evtFunc)
        bleApp->app_evtFunc(&bleNotif);
}


void esk8_gap_event_hndlr(

    esp_gap_ble_cb_event_t  event,
    esp_ble_gap_cb_param_t* param

)
{
    switch (event)
    {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(__func__, "Staring advertizing.");
            ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&adv_params));
            break;
        default:
            break;
    }
}


esk8_err_t esk8_ble_conn_clear()
{
    int dev_num = esp_ble_get_bond_device_num();
    if (dev_num < 0) return ESK8_BLE_FAILED_WL;

    if (dev_num > 0)
    {
        esp_err_t _err_code;
        esp_ble_bond_dev_t* bond_dev_l = malloc(sizeof(esp_ble_bond_dev_t)*dev_num);

        if (!bond_dev_l) return ESK8_ERR_OOM;
        _err_code = esp_ble_get_bond_device_list(&dev_num, bond_dev_l);
        if (_err_code) return ESK8_BLE_FAILED_WL;

        for (int i = 0; i < dev_num; i++)
            esp_ble_remove_bond_device(bond_dev_l[i].bd_addr);

        free(bond_dev_l);
    }

    return ESK8_SUCCESS;
}


esk8_err_t esk8_ble_conn_allow(

    uint32_t timeout_ms

)
{
    esp_ble_gap_stop_advertising();
    adv_params.adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
    esp_ble_gap_start_advertising(&adv_params);

    return ESK8_SUCCESS;
}


esk8_err_t esk8_ble_conn_block()
{
    return ESK8_SUCCESS;
}