#include <e_ride_config.h>
#include <e_ride_err.h>
#include <e_ride_ble.h>
#include <e_ride_ble_app.h>

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>
#include <nvs_flash.h>


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
 * 
 */
e_ride_err_t e_ride_ble_init();


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


e_ride_err_t e_ride_ble_init()
{
    esp_bt_controller_config_t bt_Cnfig = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

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

    ESP_ERROR_CHECK(    ret                                                         );
    ESP_ERROR_CHECK(    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT)		);
    ESP_ERROR_CHECK(    esp_bt_controller_init(&bt_Cnfig)							);
    ESP_ERROR_CHECK(    esp_bt_controller_enable(ESP_BT_MODE_BLE)					);
 
	ESP_ERROR_CHECK(    esp_bluedroid_init()										);
    ESP_ERROR_CHECK(    esp_bluedroid_enable()										);

    ESP_ERROR_CHECK(    esp_ble_gap_register_callback(e_ride_gap_event_hndlr)		);
    ESP_ERROR_CHECK(    esp_ble_gatts_register_callback(e_ride_gatts_event_hndlr)	);
    ESP_ERROR_CHECK(    esp_ble_gatts_app_register(0x00)							);
    ESP_ERROR_CHECK(    esp_ble_gatts_app_register(0x01)							);
    ESP_ERROR_CHECK(    esp_ble_gap_set_device_name(E_RIDE_BLE_DEV_NAME)			);
    ESP_ERROR_CHECK(    esp_ble_gap_config_adv_data(&adv_data)						);
    ESP_ERROR_CHECK(    esp_ble_gap_config_adv_data(&scan_rsp_data)					);

    return E_RIDE_SUCCESS;
}


void e_ride_gatts_event_hndlr(

    esp_gatts_cb_event_t event,
    esp_gatt_if_t gatts_if,
    esp_ble_gatts_cb_param_t* param

)
{
    printf("[ E_Ride ble gatts ] Got event %d on interface: %d.\n", event, gatts_if);

    if(event == ESP_GATTS_REG_EVT)
    {
        printf("[ E_Ride ble gatts ] Registering app id: %d, on interface: %d. Status: 0x%02x\n",
            param->reg.app_id,
            gatts_if,
            param->reg.status);
    }

    if(event == ESP_GATTS_CONNECT_EVT)
        esp_ble_gap_start_advertising(&adv_Params);
}


void e_ride_gap_event_hndlr(

    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param

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
