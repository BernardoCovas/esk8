#include <esk8_err.h>
#include <esk8_log.h>
#include <esk8_nvs.h>
#include <esk8_auth.h>
#include "esk8_auth_priv.h"

#include <mbedtls/md.h>

#include <stdio.h>
#include <stddef.h>
#include <string.h>


esk8_err_t esk8_auth_init(
    esk8_auth_hndl_t* hndl
)
{
    esk8_nvs_val_t sttg_val;
    esk8_err_t err_code;

    ESK8_ERRCHECK_THROW(esk8_nvs_init());

    err_code = esk8_nvs_settings_get(ESK8_NVS_AUTH_HASH, &sttg_val);
    if (err_code && err_code != ESK8_NVS_NO_VAL)
        return err_code;

    esk8_auth_cntx_t* cntx = calloc(1, sizeof(esk8_auth_cntx_t));
    if (!cntx) return ESK8_ERR_OOM;

    mbedtls_md_init(&cntx->mbtls_cntx);
    if(mbedtls_md_setup(&cntx->mbtls_cntx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1))
    {
        mbedtls_md_free(&cntx->mbtls_cntx);
        esk8_auth_deinit(hndl);
        free(cntx);

        return ESK8_ERR_OOM;
    }

    if (!err_code)
        memcpy(cntx->hash, sttg_val.auth_hash, sizeof(esk8_auth_hash_t));

    (*hndl) = cntx;

    return ESK8_SUCCESS;
}


esk8_err_t esk8_auth_register(

    esk8_auth_hndl_t* hndl,
    esk8_auth_key_t   key

)
{
    esk8_auth_cntx_t* cntx = *hndl;
    esk8_nvs_val_t sttg_val;

    if (mbedtls_md_starts(&cntx->mbtls_cntx))
        return ESK8_AUTH_ERR_HASH;

    if (mbedtls_md_update(&cntx->mbtls_cntx, key, sizeof(esk8_auth_key_t)))
        return ESK8_AUTH_ERR_HASH;

    if (mbedtls_md_finish(&cntx->mbtls_cntx, cntx->hash))
        return ESK8_AUTH_ERR_HASH;

    printf(ESK8_TAG_ATH "Registered hash: ");
    for (int i = 0; i < sizeof(esk8_auth_hash_t); i++)
        printf("%02x", cntx->hash[i]);
    printf("\n");

    memcpy(sttg_val.auth_hash, cntx->hash, sizeof(esk8_auth_hash_t));

    ESK8_ERRCHECK_THROW(esk8_nvs_settings_set(ESK8_NVS_AUTH_HASH, &sttg_val));
    ESK8_ERRCHECK_THROW(esk8_nvs_commit(ESK8_NVS_AUTH_HASH));

    sttg_val.auth_hash_n = 0;
    ESK8_ERRCHECK_THROW(esk8_nvs_settings_set(ESK8_NVS_AUTH_HASH, &sttg_val));
    ESK8_ERRCHECK_THROW(esk8_nvs_commit(ESK8_NVS_AUTH_HASH));

    return ESK8_SUCCESS;
}


esk8_err_t esk8_auth_auth(

    esk8_auth_hndl_t* hndl,
    esk8_auth_key_t   key

)
{
    esk8_auth_key_t   hash;
    esk8_auth_cntx_t* cntx = *hndl;

    if (mbedtls_md_starts(&cntx->mbtls_cntx))
        return ESK8_AUTH_ERR_HASH;

    if (mbedtls_md_update(&cntx->mbtls_cntx, key, sizeof(esk8_auth_key_t)))
        return ESK8_AUTH_ERR_HASH;

    if (mbedtls_md_finish(&cntx->mbtls_cntx, hash))
        return ESK8_AUTH_ERR_HASH;

    if (memcmp(cntx->hash, hash, sizeof(esk8_auth_key_t)) == 0)
        return ESK8_SUCCESS;

    return ESK8_AUTH_ERR_AUTH;
}

esk8_err_t esk8_auth_chunk_auth(

    esk8_auth_hndl_t* hndl,
    uint8_t*          key_chk,
    size_t            chk_len

)
{
    esk8_auth_cntx_t* auth_cntx = (*hndl);

    if ((auth_cntx->chunk_idx + chk_len) > sizeof(esk8_auth_key_t))
    {
        return ESK8_AUTH_ERR_AUTH;
    }


    return ESK8_SUCCESS;
}

esk8_err_t esk8_auth_deinit(

    esk8_auth_hndl_t* hndl

)
{
    if (!*hndl)
        return ESK8_SUCCESS;

    esk8_auth_cntx_t* cntx = *hndl;
    mbedtls_md_free(&cntx->mbtls_cntx);

    free(*hndl);
    (*hndl) = NULL;

    return ESK8_SUCCESS;
}
