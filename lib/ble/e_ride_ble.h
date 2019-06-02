#ifndef _E_RIDE_BLE_H
#define _E_RIDE_BLE_H

#include <e_ride_err.h>

#include <esp_gatts_api.h>


typedef int		e_ride_ble_app_handler_t;
typedef void	(e_ride_ble_app_evnt_cb_t)(esp_gatts_cb_event_t event, esp_ble_gatts_cb_param_t* param);


typedef struct e_ride_ble_app_t
{
	const char*						appName;
	e_ride_ble_app_evnt_cb_t* evtCb;
	e_ride_ble_app_handler_t		_appHandler; /* Internal */
} e_ride_ble_app_t;


/**
 *
 **/
e_ride_err_t e_ride_ble_init();


#endif /* _E_RIDE_BLE_H */
