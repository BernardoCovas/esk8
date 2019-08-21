#ifndef _ESK8_NVS_H
#define _ESK8_NVS_H

#define ESK8_NVS_STORAGE_NAME           "_esk8_sttng"
#define ESK8_NVS_STORAGE_KEY_SETTINGS   "_esk8_sttng"

#include <esk8_err.h>

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    ESK8_NVS_CONN_KEY,
    ESK8_NVS_CONN_ADDR,
    ESK8_NVS_IDX_MAX,
} esk8_nvs_val_idx_t;

typedef struct __attribute__((__packed__))
{
    size_t      len;
    uint8_t*    data;
    uint8_t*    data_default;
    const char* name;
}
esk8_nvs_setting_t;

extern esk8_nvs_setting_t*  esk8_nvs_setting_list;
extern uint16_t             esk8_nvs_setting_list_len;

/**
 * Initializes the encrypted NVS.
 * It still returns OK if NVS has
 * previously been initialized.
 */
esk8_err_t esk8_nvs_init();


/**
 * This function might perform a flash
 * read.
 */
esk8_err_t esk8_nvs_settings_get(

    esk8_nvs_val_idx_t  sttg_idx,
    esk8_nvs_setting_t* out_setting

);


/**
 * This function performs a flash
 * write.
 */
esk8_err_t esk8_nvs_settings_set(

    esk8_nvs_setting_t* setting

);


#endif /* _ESK8_NVS_H */
