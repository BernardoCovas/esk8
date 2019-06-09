#include <e_ride_ble.h>
#include <app_ble_srvc.h>


#define SRVC_CTRL_NAME  "Ctrl Service"
#define SRVC_CTRL_UUID  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF}


static void srvc_ctrl_evt_cb(e_ride_ble_notif_t* bleNotif);


static esp_gatt_srvc_id_t   srvc_id =
{
    .id =
    {
        .uuid =
        {
            .len = ESP_UUID_LEN_128,
            .uuid.uuid128 = SRVC_CTRL_UUID
        },
        .inst_id = 0x00 /* No idea. */
    },
    .is_primary = true
};


static e_ride_ble_char_t    srvc_ctrl_speed_char   =
{

};


static e_ride_ble_char_t    srvc_ctrl_pwr_char = 
{

};


static e_ride_ble_char_t*   srvc_ctrl_char_list[] = 
{
    &srvc_ctrl_speed_char,
    &srvc_ctrl_pwr_char
};


e_ride_ble_app_t            app_srvc_ctrl =
{
    .app_serviceName = SRVC_CTRL_NAME,
    .app_serviceId_p = &srvc_id,
    .app_numChar     = sizeof(srvc_ctrl_char_list) / sizeof(void*),
    .app_charList_p  = srvc_ctrl_char_list,
    .app_evtFunc     = &srvc_ctrl_evt_cb
};


void srvc_ctrl_evt_cb(e_ride_ble_notif_t* bleNotif)
{
    printf("[ %s ] Got event: %d\n", __func__ , bleNotif->event);    
}