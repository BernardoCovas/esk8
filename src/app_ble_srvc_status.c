#include <e_ride_ble.h>
#include <app_ble_srvc.h>

#define SRVC_STATUS_NAME    "Status Service"
#define SRVC_STATUS_UUID    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA}

static void srvc_status_evt_cb(e_ride_ble_notif_t* bleNotif);


static esp_gatt_srvc_id_t          srvc_id = {
    .id = {
        .uuid = {
            .len = ESP_UUID_LEN_128,
            .uuid.uuid128 = SRVC_STATUS_UUID
        },
        .inst_id = 0x00 /* No idea. */
    },
    .is_primary = true
};


static e_ride_ble_char_t    srvc_status_speed_char   =
{

};


static e_ride_ble_char_t    srvc_status_battery_char = 
{

};


static e_ride_ble_char_t*   srvc_status_char_list[] = 
{
    &srvc_status_speed_char,
    &srvc_status_battery_char
};


e_ride_ble_app_t        app_srvc_status =
{
    .app_serviceName = SRVC_STATUS_NAME,
    .app_serviceId_p = &srvc_id,
    .app_numChar     = sizeof(srvc_status_char_list) / sizeof(void*),
    .app_charList_p  = srvc_status_char_list,
    .app_evtFunc     = &srvc_status_evt_cb
};


void srvc_status_evt_cb(e_ride_ble_notif_t* bleNotif)
{
    printf("[ %s ] Got event: %d\n", __func__ , bleNotif->event);
}