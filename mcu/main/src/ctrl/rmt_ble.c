#include <rmt_ble.h>

#include <esp_log.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gattc_api.h>
#include <nvs_flash.h>


#define ESK8_BLE_RMT_NAME "Esk8 Remote"

esk8_err_t app_ctrl_init();
esk8_err_t app_ctrl_update_speed(esk8_ps2_mvmnt_t mvmtPacket);
esk8_err_t app_ctrl_update_pwr(bool pwr);

static void  app_ctrl_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void  app_ctrl_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);


esk8_err_t app_ctrl_init(

    void

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
    bt_Cnfg.controller_task_prio = 1;

    ESP_ERROR_CHECK(    ret                                                         );
    ESP_ERROR_CHECK(    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT)       );
    ESP_ERROR_CHECK(    esp_bt_controller_init(&bt_Cnfg)                            );
    ESP_ERROR_CHECK(    esp_bt_controller_enable(ESP_BT_MODE_BLE)                   );
 
    ESP_ERROR_CHECK(    esp_bluedroid_init()                                        );
    ESP_ERROR_CHECK(    esp_bluedroid_enable()                                      );

    ESP_ERROR_CHECK(    esp_ble_gap_register_callback(app_ctrl_gap_cb)              );
    ESP_ERROR_CHECK(    esp_ble_gap_set_device_name(ESK8_BLE_RMT_NAME)            );
    ESP_ERROR_CHECK(    esp_ble_gattc_register_callback(app_ctrl_gattc_cb)          );

    return ESK8_SUCCESS;
}


esk8_err_t app_ctrl_update_speed(

    esk8_ps2_mvmnt_t mvmtPacket

)
{
    static uint8_t speed = 0;

    speed += mvmtPacket.x;
    printf("UPDATE_SPEED Updated to: 0x%02x\n", speed);

    return ESK8_SUCCESS;
}


esk8_err_t app_ctrl_update_pwr(

    bool pwr

)
{
    return ESK8_SUCCESS;
}


void app_ctrl_gap_cb(
    
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param
    
)
{
    printf("GAP Got event: %d\n", event);
}


void app_ctrl_gattc_cb(

    esp_gattc_cb_event_t event,
    esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t *param

)
{
    printf("GATTC Got event: %d\n", event);
}


