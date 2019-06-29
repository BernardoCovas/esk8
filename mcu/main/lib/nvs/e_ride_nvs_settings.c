#include <esk8_nvs.h>

#include <nvs.h>
#include <nvs_flash.h>

#include <stdbool.h>


static esk8_nvs_settings_t    esk8_nvs_settings = {0};
static nvs_handle_t             esk8_nvs_handle   = 0;
static bool                     esk8_nvs_loaded   = false;


esk8_err_t esk8_nvs_init()
{
    if (esk8_nvs_handle > 0)
        return ESK8_SUCCESS;

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

    return ESK8_SUCCESS;
}


esk8_err_t esk8_nvs_settings_get(

    esk8_nvs_settings_t* out_settings

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