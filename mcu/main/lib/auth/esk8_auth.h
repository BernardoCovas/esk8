#ifndef _ESK8_AUTH_H
#define _ESK8_AUTH_H

#include <esk8_err.h>

#include <stdint.h>
#include <stdbool.h>


typedef uint8_t esk8_auth_key_t [32];
typedef uint8_t esk8_auth_hash_t[32];
typedef void*   esk8_auth_hndl_t;

/**
 * Initializes the auth cntx,
 * and the underlying crypto cntx.
 * Needs to be called before any
 * other function.
 */
esk8_err_t
esk8_auth_init(
    esk8_auth_hndl_t* hndl);

/**
 * Registers the new key as the
 * auth key.
 */
esk8_err_t
esk8_auth_register(
    esk8_auth_hndl_t* hndl,
    esk8_auth_key_t   key);

/**
 *
 */
esk8_err_t
esk8_auth_chunk_auth(
    esk8_auth_hndl_t* hndl,
    uint8_t*          key_chk,
    size_t            chk_len);

/**
 *
 */
esk8_err_t
esk8_auth_auth(
    esk8_auth_hndl_t* hndl,
    esk8_auth_key_t   key);

/**
 *
 */
esk8_err_t
esk8_auth_deinit(
    esk8_auth_hndl_t* hndl);


#endif /* _ESK8_AUTH_H */
