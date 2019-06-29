#ifndef _ESK8_NVS_H
#define _ESK8_NVS_H

#define ESK8_NVS_STORAGE_NAME           "_esk8_sttng"
#define ESK8_NVS_STORAGE_KEY_SETTINGS   "_esk8_sttng"

#include <esk8_err.h>

#include <stdint.h>


typedef struct __attribute__((__packed__))
{
    uint8_t esk8_ble_peer_addr[6];
}
esk8_nvs_settings_t;


/**
 * Initializes the encrypted NVS.
 * It still returns OK if NVS has
 * previously been initialized.
 */
esk8_err_t esk8_nvs_init();


/**
 * This function performs a flash
 * read.
 */
esk8_err_t esk8_nvs_settings_get(

    esk8_nvs_settings_t* out_settings

);


/**
 * This function performs a flash
 * write.
 */
esk8_err_t esk8_nvs_settings_set(

    esk8_nvs_settings_t* settings

);


#endif /* _ESK8_NVS_H */
