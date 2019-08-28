#include <esk8_auth.h>
#include <esk8_nvs.h>

#include <string.h>


esk8_err_t esk8_auth_init(

    esk8_auth_hndl_t* hndl

)
{
    esk8_nvs_val_t sttg_val;
    ESK8_ERRCHECK_THROW(esk8_nvs_init());
    ESK8_ERRCHECK_THROW(esk8_nvs_settings_get(ESK8_NVS_CONN_KEY, &sttg_val));

    memcpy(hndl->key, sttg_val.conn_key, sizeof(sttg_val.conn_key));

    return ESK8_SUCCESS;
}


esk8_err_t esk8_auth_auth(

    esk8_auth_hndl_t* hndl,
    esk8_auth_key_t   key

)
{
    return ESK8_SUCCESS;
}


esk8_err_t esk8_auth_deauth(

    esk8_auth_hndl_t* hndl

)
{
    return ESK8_SUCCESS;
}


esk8_err_t esk8_auth_deinit(

    esk8_auth_hndl_t* hndl

)
{
    return ESK8_SUCCESS;
}