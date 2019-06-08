#include <e_ride_config.h>
#include <e_ride_err.h>
#include <e_ride_ble.h>

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>
#include <nvs_flash.h>


#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include <string.h>
#include <stddef.h>


static uint8_t adv_service_uuid128[32] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xEE, 0x00, 0x00, 0x00,
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};


static esp_ble_adv_params_t adv_Params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};


static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};


static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};


/**
 * Global one and only BLE allocation.
 */
static e_ride_ble_t bleHandler = {0};


/**
 * 
 */
void e_ride_gatts_event_hndlr(

    esp_gatts_cb_event_t event,
    esp_gatt_if_t gatts_if,
    esp_ble_gatts_cb_param_t *param

);


/**
 * 
 */
void e_ride_gap_event_hndlr(

    esp_gap_ble_cb_event_t  event,
    esp_ble_gap_cb_param_t* param

);


e_ride_err_t e_ride_ble_init(

    e_ride_ble_config_t*    bleCnfg

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

    ESP_ERROR_CHECK(    esp_ble_gap_register_callback(e_ride_gap_event_hndlr)       );
    ESP_ERROR_CHECK(    esp_ble_gatts_register_callback(e_ride_gatts_event_hndlr)   );
    ESP_ERROR_CHECK(    esp_ble_gap_set_device_name(E_RIDE_BLE_DEV_NAME)            );
    ESP_ERROR_CHECK(    esp_ble_gap_config_adv_data(&adv_data)                      );
    ESP_ERROR_CHECK(    esp_ble_gap_config_adv_data(&scan_rsp_data)                 );

    return E_RIDE_SUCCESS;
}


e_ride_err_t e_ride_ble_close()
{
    if (!bleHandler.p_appList)
        return E_RIDE_SUCCESS;

    for (int i=0; i<bleHandler.appNum; i++)
    {
        if (bleHandler.p_appList[i]->_evntQueue)
            vQueueDelete((QueueHandle_t)bleHandler.p_appList[i]->_evntQueue);
    }

    free(bleHandler.p_appList);

    return E_RIDE_SUCCESS;
}


e_ride_err_t e_ride_ble_register_apps(

    uint16_t           numApps,
    e_ride_ble_app_t** p_appList

)
{
    static size_t currAppId = 0;

    if (!numApps || !p_appList)
        return E_RIDE_ERR_INVALID_PARAM;

    if (currAppId > 0)
        /**
         * This function was already called.
         * We don't want that. It can only
         * be called once.
         */
        return E_RIDE_BLE_INIT_REINIT;

    /**
     * Create the app list, and event queues.
     */
    e_ride_ble_app_t** appList = calloc(numApps, sizeof(e_ride_ble_app_t));
    if (!appList)
        return E_RIDE_ERR_OOM;

    /**
     * Copy all the app's addresses to the bleHandler.
     * They are assumed to last for the time ble is
     * running.
     * */
    memcpy((void*)bleHandler.p_appList, (void*)appList, sizeof(e_ride_ble_app_t*) * numApps);

    for (int i=0; i<numApps; i++)
    {
        appList[i]->_evntQueue = (void*) xQueueCreate(
            bleHandler.bleCnfg.evtQueueLen, sizeof(e_ride_ble_notif_t));

        if (!appList[i]->_evntQueue)
        {
            e_ride_ble_close();
            return E_RIDE_ERR_OOM;
        }
    }

    /**
     * Only register the apps with ble once we
     * know we have memory, after allocating
     * all event queues.
     */
    for (size_t i = 0; i<numApps; i++)
        ESP_ERROR_CHECK(esp_ble_gatts_app_register(currAppId++));

    return E_RIDE_SUCCESS;
}


void e_ride_gatts_event_hndlr(

    esp_gatts_cb_event_t event,
    esp_gatt_if_t gatts_if,
    esp_ble_gatts_cb_param_t* param

)
{
    QueueHandle_t evtQueue;
    e_ride_ble_notif_t bleNotif;
    uint16_t appId;

    switch (event)
    {
        case ESP_GATTS_REG_EVT:
            appId = param->reg.app_id;

            /**
             * The app id is the index.
             * Check if some some reason
             * the index is unexpected.
             */
            if (appId > bleHandler.appNum)
                return;

            /**
             * The handler is the interface associated
             * with the app.
             */
            bleHandler.p_appList[appId]->_appHandlr = appId;

            bleNotif.event = E_RIDE_BLE_EVT_REGISTD;
            bleNotif.param.reg.appHandler = (e_ride_ble_app_handler_t)appId;
            break;

        case ESP_GATTS_CONNECT_EVT:
            /**
             * Esp stops advertising.
             * This restarts it.
             */
            esp_ble_gap_start_advertising(&adv_Params);

            bleNotif.event = E_RIDE_BLE_EVT_NEWCONN;
            bleNotif.param.connect.devHandler = param->connect.conn_id;
            memcpy(
                bleNotif.param.connect.mac,
                param->connect.remote_bda, 6);
            break;

        default:
            return;
    }

    /**
     * Find which app this event is meant to.
     */
    for (int i=0; i<bleHandler.appNum; i++)
    {
        if (bleHandler.p_appList[i]->_appHandlr == gatts_if)
        {
            evtQueue = (QueueHandle_t)bleHandler.p_appList[i]->_evntQueue;
            xQueueSend(evtQueue, &bleNotif, portMAX_DELAY);
        }
    }
}


void e_ride_gap_event_hndlr(

    esp_gap_ble_cb_event_t  event,
    esp_ble_gap_cb_param_t* param

)
{
    switch (event)
    {
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&adv_Params);
            break;
        default:
            break;
    }

    printf("[ E_Ride ble gap ] Got event: 0x%02x\n", event);
}


e_ride_err_t e_ride_ble_app_evnt_await(

    e_ride_ble_app_handler_t appHndlr,
    e_ride_ble_notif_t*      appNotif,
    uint32_t                 timeout_ms

)
{
    if (!bleHandler.p_appList)
        return E_RIDE_BLE_INIT_NOINIT;

    if  (
            appHndlr >= bleHandler.appNum   ||
            !bleHandler.p_appList[appHndlr]
        )
        return E_RIDE_ERR_INVALID_PARAM;

    if (xQueueReceive(
        bleHandler.p_appList[appHndlr]->_evntQueue,
        appNotif,
        timeout_ms / portTICK_PERIOD_MS) != pdTRUE)
    {
        return E_RIDE_BLE_NOTF_TIMEOUT;
    }

    return E_RIDE_SUCCESS;
}