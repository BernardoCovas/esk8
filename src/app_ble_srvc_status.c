#include <e_ride_ble.h>
#include <app_ble_app.h>


static e_ride_ble_char_t    srvc_status_battery_char = {0};
static e_ride_ble_char_t    srvc_status_speed_char   = {0};

static e_ride_ble_char_t*   srvc_char_list[] = {
    &srvc_status_battery_char,
    &srvc_status_speed_char
};

static e_ride_ble_app_t     srvc_status = {
    .app_numChar = sizeof(srvc_char_list) / sizeof(void*),
    .app_charList_p = &srvc_char_list
};


e_ride_ble_app_t* app_get_srvc_status()
{
    return &srvc_status;
}
