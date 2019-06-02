#ifndef _E_RIDE_BLE_APP_H
#define _E_RIDE_BLE_APP_H


#include <e_ride_ble.h>

e_ride_ble_app_evnt_cb_t e_ride_status_cb;
e_ride_ble_app_evnt_cb_t e_ride_battery_cb;

#define e_ride_ble_app_status 			\
{										\
	.appName	= "E-Ride Status",		\
	.evtCb		= &e_ride_status_cb		\
}


#define e_ride_ble_app_battery 			\
{										\
	.appName	= "E-Ride Battery",		\
	.evtCb		= &e_ride_battery_cb	\
}


#define E_RIDE_BLE_APPS_LIST()			\
{ 										\
	e_ride_ble_app_status,				\
	e_ride_ble_app_battery				\
};


#endif /* _E_RIDE_BLE_APP_H */