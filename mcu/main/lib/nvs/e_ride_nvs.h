#ifndef _E_RIDE_NVS_H
#define _E_RIDE_NVS_H

#define ESK8_NVS_STORAGE_NAME           "_esk8_sttng"
#define ESK8_NVS_STORAGE_KEY_SETTINGS   "_esk8_sttng"

#include <e_ride_err.h>

#include <stdint.h>


typedef struct __attribute__((__packed__))
{
    uint8_t esk8_ble_peer_addr[6];
}
e_ride_nvs_settings_t;


/**
 * Initializes the encrypted NVS.
 * It still returns OK if NVS has
 * previously been initialized.
 */
e_ride_err_t e_ride_nvs_init();


/**
 * This function performs a flash
 * read.
 */
e_ride_err_t e_ride_nvs_settings_get(

    e_ride_nvs_settings_t* out_settings

);


/**
 * This function performs a flash
 * write.
 */
e_ride_err_t e_ride_nvs_settings_set(

    e_ride_nvs_settings_t* settings

);


#endif /* _E_RIDE_NVS_H */
