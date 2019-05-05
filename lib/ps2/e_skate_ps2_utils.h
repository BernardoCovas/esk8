#ifndef _E_SKATE_PS2_UTILS_H
#define _E_SKATE_PS2_UTILS_H

#include "e_skate_ps2.h"
#include <e_skate_err.h>


/**
 * Adds a bit to the value in `ps2Handle`.
 * This is expected to start with the least
 * significant bit, and contains the start,
 * parity and stop bits. For example, to 
 * form the value 94, we would do, in order:
 * `{1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0}`.
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
 * Checks The integrity of the 
 * handler's internal value.
 * If it is as expected, returns
 * `E_SKATE_SUCCESS`. If wrong, returns
 * `E_SKATE_PS2_ERR_INVALID_STATE`.
 **/
e_skate_err_t e_skate_ps2_check_frame(

    e_skate_ps2_handle_t* ps2Handle

);


#endif /* _E_SKATE_PS2_UTILS_H */