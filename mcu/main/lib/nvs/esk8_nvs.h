#ifndef _ESK8_NVS_H
#define _ESK8_NVS_H

#define ESK8_NVS_STORAGE_NAME "esk8"

#include <esk8_err.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


typedef enum
{
    ESK8_NVS_AUTH_HASH,
    ESK8_NVS_AUTH_HASH_N,
    ESK8_NVS_CONN_ADDR,
    ESK8_NVS_IDX_MAX,
}
esk8_nvs_val_idx_t;

typedef union
{
    uint32_t  auth_hash_n;
    uint8_t   auth_hash[32];
    uint8_t   conn_addr[6];
}
esk8_nvs_val_t;

typedef struct
{
    const char*     nvs_key;
    const size_t    nvs_len;
    void*           nvs_val;

    void*           __nvs_mem;
}
esk8_nvs_setting_t;

extern esk8_nvs_setting_t esk8_nvs_setting_list[ESK8_NVS_IDX_MAX];


/**
 * Initializes the encrypted NVS.
 * It still returns OK if NVS has
 * previously been initialized.
 * Performs a flash read.
 */
esk8_err_t esk8_nvs_init();


/**
 * This function does not perform
 * a flash read.
 */
esk8_err_t esk8_nvs_settings_get(

    esk8_nvs_val_idx_t  sttg_idx,
    esk8_nvs_val_t*     out_val

);


/**
 * This function does not perform
 * a flash write.
 */
esk8_err_t esk8_nvs_settings_set(

    esk8_nvs_val_idx_t  sttg_idx,
    esk8_nvs_val_t*     sttg_val

);


/**
 * This function does perform a
 * flash write.
 * if `sttg_idx` is `ESK8_NVS_IDX_MAX`,
 * saves all settings.
 */
esk8_err_t esk8_nvs_commit(

    esk8_nvs_val_idx_t  sttg_idx

);


/**
 * Frees alll allocated memory and sets
 * internal values to NULL.
 */
esk8_err_t esk8_nvs_settings_deinit();



#endif /* _ESK8_NVS_H */
