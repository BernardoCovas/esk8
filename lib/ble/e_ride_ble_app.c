#include <e_ride_ble.h>
#include <e_ride_ble_app.h>

#include <stdio.h>


void e_ride_status_cb(

	esp_gatts_cb_event_t event,
	esp_ble_gatts_cb_param_t* param

)
{
	printf("Event %d handled by e_ride_status_cb().\n", event);
}


void e_ride_battery_cb(

	esp_gatts_cb_event_t event,
	esp_ble_gatts_cb_param_t* param

)
{
	printf("Event %d handled by e_ride_battery_cb().\n", event);
}