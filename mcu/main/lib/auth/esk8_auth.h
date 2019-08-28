#ifndef _ESK8_AUTH_H
#define _ESK8_AUTH_H

#include <esk8_err.h>

#include <stdint.h>
#include <stdbool.h>


typedef uint8_t esk8_auth_key_t[32];

typedef struct
{
    esk8_auth_key_t key;
    uint16_t        sess;
} esk8_auth_hndl_t;


/**
 *
 */
esk8_err_t
esk8_auth_init(
    esk8_auth_hndl_t* hndl);

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
esk8_auth_deauth(
    esk8_auth_hndl_t* hndl);

/**
 * 
 */
esk8_err_t
esk8_auth_deinit(
    esk8_auth_hndl_t* hndl);


#endif /* _ESK8_AUTH_H */
