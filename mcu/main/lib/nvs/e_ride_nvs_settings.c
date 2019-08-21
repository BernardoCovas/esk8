#include <esk8_nvs.h>

#include <nvs.h>
#include <nvs_flash.h>

#include <stdbool.h>


esk8_nvs_setting_t  esk8_nvs_setting_list[ESK8_NVS_IDX_MAX] = {
    [ESK8_NVS_CONN_KEY]     = { .len = 32,  .data = NULL, .data_default = "", .name = "BLE Auth key" },
    [ESK8_NVS_CONN_ADDR]    = { .len = 6,   .data = NULL, .data_default = "", .name = "BLE Conn addr"}
};
        uint16_t        esk8_nvs_setting_list_len   = sizeof(esk8_nvs_setting_list) / sizeof(esk8_nvs_setting_list[0]);
static  nvs_handle_t    esk8_nvs_handle             = 0;
static  size_t          esk8_nvs_data_len           = 0;
static  uint8_t*        esk8_nvs_data               = NULL;


esk8_err_t esk8_nvs_init()
{
    if (esk8_nvs_handle > 0)
        return ESK8_SUCCESS;

    if (esk8_nvs_data)
    {
        free(esk8_nvs_data);
        esk8_nvs_data_len = 0;
    }

    esp_err_t errCode = nvs_flash_init();
    if (errCode == ESP_ERR_NVS_NO_FREE_PAGES || errCode == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        errCode = nvs_flash_erase();
        if (errCode)
            return ESK8_NVS_NOT_AVAILABLE;

        errCode = nvs_flash_init();
        if (errCode)
            return ESK8_NVS_NOT_AVAILABLE;
    }

    errCode = nvs_open(ESK8_NVS_STORAGE_NAME, NVS_READWRITE, &esk8_nvs_handle);
    if (errCode)
        return ESK8_NVS_NOT_AVAILABLE;

    errCode = nvs_get_blob(esk8_nvs_handle,
        ESK8_NVS_STORAGE_KEY_SETTINGS, NULL, &esk8_nvs_data_len);
    
    if (errCode == ESP_ERR_NVS_NOT_FOUND)
        return ESK8_NVS_NO_SETTINGS;
    else if(errCode)
        return ESK8_NVS_NOT_AVAILABLE;

    if (esk8_nvs_data_len == 0)
        return ESK8_NVS_NOT_AVAILABLE;
    
    size_t expected_data_len = 0;
    for (int i = 0; i < ESK8_NVS_IDX_MAX; i++)
        expected_data_len += esk8_nvs_setting_list[i].len;

    if (expected_data_len != esk8_nvs_data_len)
        return ESK8_NVS_WRONG_SIZE;

    esk8_nvs_data = (uint8_t*)malloc(esk8_nvs_data_len);
    if (!esk8_nvs_data)
        return ESK8_ERR_OOM;

/* Memory is now allocated. Needs to be cleared before return. */
esk8_err_t return_err_code = ESK8_SUCCESS;
#define RETURN(X) do { return_err_code = (X); goto __nvs_err_return; } while(0)

    errCode = nvs_get_blob(esk8_nvs_handle,
        ESK8_NVS_STORAGE_KEY_SETTINGS, (void*)esk8_nvs_data, &esk8_nvs_data_len);

    if (errCode)
        RETURN(ESK8_NVS_NOT_AVAILABLE);

    size_t expected_size = 0;
    for (int i = 0; i < ESK8_NVS_IDX_MAX; i++)
        expected_size += esk8_nvs_setting_list[i].len;

    if (expected_size != esk8_nvs_data_len)
        RETURN(ESK8_NVS_WRONG_SIZE);

#undef RETURN

    size_t curr_idx = 0;
    for (int i = 0; i < ESK8_NVS_IDX_MAX; i++)
    {
        esk8_nvs_setting_list[i].data = &esk8_nvs_data[i];
        i += esk8_nvs_setting_list[i].len;
    }

    return ESK8_SUCCESS;


__nvs_err_return:

    if (esk8_nvs_data) free(esk8_nvs_data);
    esk8_nvs_data_len = 0;

    return return_err_code;
}


esk8_err_t esk8_nvs_write_defaults()
{

}


esk8_err_t esk8_nvs_settings_get(

    esk8_nvs_val_idx_t  sttg_idx,
    esk8_nvs_setting_t* out_setting

)
{
    esp_err_t errCode;

    if (esk8_nvs_handle == 0)
        return ESK8_NVS_NOT_INIT;

    if (esk8_nvs_loaded)
    {
        (*out_settings) = esk8_nvs_settings;
        return ESK8_SUCCESS;
    }

    size_t sttgs_size   = sizeof(esk8_nvs_settings_t);
    size_t sttgs_size_r = 0;

    errCode = nvs_get_blob(esk8_nvs_handle,
        ESK8_NVS_STORAGE_KEY_SETTINGS, NULL, &sttgs_size_r);
    
    if (errCode == ESP_ERR_NVS_NOT_FOUND)
        return ESK8_NVS_NO_SETTINGS;

    if (sttgs_size != sttgs_size_r)
        return ESK8_NVS_WRONG_SIZE;

    errCode = nvs_get_blob(esk8_nvs_handle,
        ESK8_NVS_STORAGE_KEY_SETTINGS, (void*)out_settings, &sttgs_size_r);

    if (errCode)
        return ESK8_NVS_NOT_AVAILABLE;

    esk8_nvs_loaded = true;
    return ESK8_SUCCESS;
}


esk8_err_t esk8_nvs_settings_set(

    esk8_nvs_settings_t* settings

)
{
    if (esk8_nvs_handle == 0)
        return ESK8_NVS_NOT_INIT;

    esp_err_t errCode;
    size_t sttgs_size = sizeof(esk8_nvs_settings_t);

    errCode = nvs_set_blob(esk8_nvs_handle,
        ESK8_NVS_STORAGE_KEY_SETTINGS, (void*)settings, sttgs_size);

    if (!errCode)
    {
        esk8_nvs_settings = (*settings);
        esk8_nvs_loaded   = true;
        return ESK8_SUCCESS;
    }

    switch (errCode)
    {
        case ESP_ERR_NVS_INVALID_HANDLE:
            return ESK8_NVS_NOT_INIT;
        case ESP_ERR_NVS_NOT_ENOUGH_SPACE:
            return ESK8_NVS_FULL;
        default:
            return ESK8_NVS_NOT_AVAILABLE;
    }
}