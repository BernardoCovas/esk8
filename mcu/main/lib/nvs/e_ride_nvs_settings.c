#include <esk8_nvs.h>
#include <esk8_log.h>

#include <nvs.h>
#include <nvs_flash.h>

#include <string.h>
#include <stdbool.h>


esk8_nvs_setting_t  esk8_nvs_setting_list[ESK8_NVS_IDX_MAX] = {
    [ESK8_NVS_AUTH_HASH]    = { .nvs_len = 32,  .nvs_key = "ble_auth_hash", .nvs_val = NULL, .__nvs_mem = NULL  },
    [ESK8_NVS_AUTH_HASH_N]  = { .nvs_len = 4,   .nvs_key = "ble_auth_n",    .nvs_val = NULL, .__nvs_mem = NULL  },
    [ESK8_NVS_CONN_ADDR]    = { .nvs_len = 6,   .nvs_key = "ble_conn_add",  .nvs_val = NULL, .__nvs_mem = NULL  }
};
static  nvs_handle_t    esk8_nvs_handle = 0;


esk8_err_t esk8_nvs_init()
{
    if (esk8_nvs_handle > 0)
        return ESK8_OK;

    esp_err_t esp_err;
    esp_err = nvs_flash_init();
    if (esp_err == ESP_ERR_NVS_NO_FREE_PAGES || esp_err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        esp_err = nvs_flash_erase();
        if (esp_err)
            return ESK8_ERR_NVS_NOT_AVAILABLE;

        esp_err = nvs_flash_init();
        if (esp_err)
            return ESK8_ERR_NVS_NOT_AVAILABLE;
    }

    esp_err = nvs_open(ESK8_NVS_STORAGE_NAME, NVS_READWRITE, &esk8_nvs_handle);
    if (esp_err)
        return ESK8_ERR_NVS_NOT_AVAILABLE;

    for (int i = 0; i < ESK8_NVS_IDX_MAX; i++)
    {
        size_t data_len = 0;
        esk8_nvs_setting_t* sttg = &esk8_nvs_setting_list[i];

        sttg->__nvs_mem = malloc(sttg->nvs_len);
        if (!sttg->__nvs_mem)
        {
            esk8_nvs_settings_deinit();
            return ESK8_ERR_OOM;
        }

        esp_err = nvs_get_blob(esk8_nvs_handle,
            sttg->nvs_key, NULL, &data_len);

        if (esp_err)
            continue;

        if (data_len != sttg->nvs_len)
            continue;

        esp_err = nvs_get_blob(esk8_nvs_handle,
            sttg->nvs_key, sttg->__nvs_mem, &data_len);

        if (esp_err)
        {
            sttg->nvs_val = NULL;
            continue;
        }

        esk8_log_D(ESK8_TAG_NVS, "Got value: %s\n", sttg->nvs_key);
        sttg->nvs_val = sttg->__nvs_mem;
    }

    return ESK8_OK;
}


esk8_err_t esk8_nvs_settings_get(

    esk8_nvs_val_idx_t  sttg_idx,
    esk8_nvs_val_t*     out_val

)
{
    if (esk8_nvs_handle == 0)
        return ESK8_ERR_NVS_NOT_INIT;

    if (sttg_idx >= ESK8_NVS_IDX_MAX || sttg_idx < 0)
        return ESK8_ERR_NVS_NO_IDX;

    esk8_nvs_setting_t* sttg = &esk8_nvs_setting_list[sttg_idx];

    if (!sttg->nvs_val)
        return ESK8_ERR_NVS_NO_VAL;

    memcpy(out_val, sttg->nvs_val, sttg->nvs_len);
    return ESK8_OK;
}


esk8_err_t esk8_nvs_settings_set(

    esk8_nvs_val_idx_t  sttg_idx,
    esk8_nvs_val_t*     sttg_val

)
{
    if (esk8_nvs_handle == 0)
        return ESK8_ERR_NVS_NOT_INIT;

    if (sttg_idx >= ESK8_NVS_IDX_MAX || sttg_idx < 0)
        return ESK8_ERR_NVS_NO_IDX;

    esk8_nvs_setting_t* sttg = &esk8_nvs_setting_list[sttg_idx];

    if (!sttg->nvs_val)
        sttg->nvs_val = sttg->__nvs_mem;

    memcpy(sttg->nvs_val, sttg_val, sttg->nvs_len);
    return ESK8_OK;
}


esk8_err_t esk8_nvs_commit(

    esk8_nvs_val_idx_t  sttg_idx

)
{
    if (esk8_nvs_handle == 0)
        return ESK8_ERR_NVS_NOT_INIT;

    if (sttg_idx > ESK8_NVS_IDX_MAX || sttg_idx < 0)
        return ESK8_ERR_NVS_NO_IDX;

    if (sttg_idx == ESK8_NVS_IDX_MAX)
    {
        for (int i = 0; i < ESK8_NVS_IDX_MAX; i++)
        {
            esk8_nvs_setting_t* sttg = &esk8_nvs_setting_list[i];

            if (nvs_set_blob(esk8_nvs_handle, sttg->nvs_key, sttg->nvs_val, sttg->nvs_len))
                return ESK8_ERR_NVS_ERR_WRITE;
        }

        goto __nvs_commit;
    }

    esk8_nvs_setting_t* sttg = &esk8_nvs_setting_list[sttg_idx];
    esp_err_t errCode = nvs_set_blob(esk8_nvs_handle, sttg->nvs_key, sttg->nvs_val, sttg->nvs_len);

    if (errCode)
        return ESK8_ERR_NVS_ERR_WRITE;

__nvs_commit:

    if (nvs_commit(esk8_nvs_handle))
        return ESK8_ERR_NVS_ERR_WRITE;

    return ESK8_OK;
}


esk8_err_t esk8_nvs_settings_deinit()
{
    for (int i = 0; i < ESK8_NVS_IDX_MAX; i++)
    {
        esk8_nvs_setting_t* sttg = &esk8_nvs_setting_list[i];
        if (sttg->__nvs_mem)
        {
            free(sttg->__nvs_mem);
            sttg->__nvs_mem = NULL;
        }

        sttg->nvs_val = NULL;
    }

    return ESK8_OK;
}