#ifndef _ESK8_PS2_UTILS_H
#define _ESK8_PS2_UTILS_H

#include "esk8_ps2.h"
#include <esk8_err.h>


typedef struct
{
    bool    bit;
    double  bitInterval_s; /* Time between this bit and the last, in seconds. */
} esk8_ps2_bit_t;


/**
 * Adds a bit to the value in `ps2Pkt`.
 * This is expected to start with the least
 * significant bit, and contains the start,
 * parity and stop bits. For example, to 
 * form the value 94, we would do, in order:
 * `{1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0}`.
 * Returns `ESK8_PS2_ERR_VALUE_READY` if
 * the value is complete. If we try to add
 * a value to an already complete handle,
 * it is not added and returns the same.
 **/
esk8_err_t esk8_ps2_add_bit(

    esk8_ps2_pkt_t*  ps2Pkt,
    bool                value

);


/**
 * Takes a bit from the value in `ps2Pkt`.
 * Returns `ESK8_PS2_ERR_VALUE_READY` if
 * the value is done complete. If we try to add
 * a value to an already completely empty
 * packet, returns the same and does nothing.
 **/
esk8_err_t esk8_ps2_take_bit(

    esk8_ps2_pkt_t*  ps2Pkt,
    bool*               outValue

);


/**
 * Checks The integrity of the 
 * packet's internal value.
 * If it is as expected, returns
 * `ESK8_SUCCESS`. If wrong, returns
 * `ESK8_PS2_ERR_INVALID_STATE`.
 **/
esk8_err_t esk8_ps2_check_pkt(

    esk8_ps2_pkt_t* ps2Pkt

);


/**
 * Resets the internal value and index
 * of `ps2Pkt`.
 **/
void esk8_ps2_reset_pkt(

    esk8_ps2_pkt_t* ps2Pkt

);


/**
 * Returns the parity val.
 **/
bool esk8_ps2_get_parity(

    uint8_t x

);


#endif /* _ESK8_PS2_UTILS_H */
