#ifndef _E_SKATE_PS2_UTILS_H
#define _E_SKATE_PS2_UTILS_H

#include "e_skate_ps2.h"
#include <e_skate_err.h>


/**
 * Adds a bit to the value in `ps2Handle`.
 * Returns `E_SKATE_PS2_ERR_VALUE_READY` if
 * the value is complete. If we try to add
 * a value to an already complete handle,
 * it is not added and returns the same.
 **/
e_skate_err_t e_skate_ps2_add_bit(

    e_skate_ps2_handle_t* ps2Handle,
    bool value

);


/**
 * Resets the internal value and index
 * of `ps2Handle`.
 **/
void e_skate_ps2_reset_handle(

    e_skate_ps2_handle_t* ps2Handle

);


#endif /* _E_SKATE_PS2_UTILS_H */
