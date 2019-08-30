#ifndef _ESK8_AUTH_PRIV_H
#define _ESK8_AUTH_PRIV_H

#include "esk8_auth.h"

#include <mbedtls/md.h>


typedef struct
{
    esk8_auth_key_t hash;
    uint16_t        conn;

    mbedtls_md_context_t mbtls_cntx;
} esk8_auth_cntx_t;


#endif  /* _ESK8_AUTH_PRIV_H */
