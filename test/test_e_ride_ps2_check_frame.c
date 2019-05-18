#include "test_e_ride.h"
#include <e_ride_ps2.h>
#include <e_ride_ps2_utils.h>

#include <unity.h>


void test_e_ride_ps2_check_frame()
{
    
    e_ride_ps2_pkt_t ps2Pkt;
    e_ride_ps2_reset_pkt(&ps2Pkt);

    uint8_t testFrames[][11] =  {
        {/* Start (Should be 0) */ 0, /* Data */ 0, 0, 0, 0, 0, 0, 0, 0, /* Parity (Right) */ 1, /* Stop (Should be 1) */ 1},
        {/* Start (Should be 0) */ 0, /* Data */ 0, 0, 0, 0, 0, 0, 0, 0, /* Parity (Wrong) */ 0, /* Stop (Should be 1) */ 1},

        {/* Start (Should be 0) */ 0, /* Data */ 1, 1, 1, 1, 1, 0, 1, 1, /* Parity (Right) */ 0, /* Stop (Should be 1) */ 1},
        {/* Start (Should be 0) */ 0, /* Data */ 1, 1, 1, 1, 1, 0, 1, 1, /* Parity (Wrong) */ 1, /* Stop (Should be 1) */ 1},

        {/* Start (Should be 0) */ 0, /* Data */ 1, 1, 1, 1, 1, 0, 1, 0, /* Parity (Right) */ 1, /* Stop (Should be 1) */ 1},
        {/* Start (Should be 0) */ 0, /* Data */ 1, 1, 1, 1, 1, 0, 1, 0, /* Parity (Wrong) */ 0, /* Stop (Should be 1) */ 1},

        {/* Start (Should be 0) */ 0, /* Data */ 1, 1, 1, 1, 1, 1, 1, 1, /* Parity (Right) */ 1, /* Stop (Should be 1) */ 1},
        {/* Start (Should be 0) */ 0, /* Data */ 1, 1, 1, 1, 1, 1, 1, 1, /* Parity (Wrong) */ 0, /* Stop (Should be 1) */ 1},

        {/* Start (Should be 0) */ 0, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Right) */ 0, /* Stop (Should be 1) */ 1},
        {/* Start (Should be 0) */ 0, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Wrong) */ 1, /* Stop (Should be 1) */ 1},

        {/* Start (Should be 0) */ 0, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Right) */ 0, /* Stop (Should be 1) */ 1},
        {/* Start (Should be 0) */ 1, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Right) */ 0, /* Stop (Should be 1) */ 1},

        {/* Start (Should be 0) */ 0, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Right) */ 0, /* Stop (Should be 1) */ 1},
        {/* Start (Should be 0) */ 0, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Right) */ 0, /* Stop (Should be 1) */ 0},
    };

    esp_err_t expectedErrCode[] = {
        E_RIDE_SUCCESS,
        E_RIDE_PS2_ERR_INVALID_STATE,

        E_RIDE_SUCCESS,
        E_RIDE_PS2_ERR_INVALID_STATE,

        E_RIDE_SUCCESS,
        E_RIDE_PS2_ERR_INVALID_STATE,

        E_RIDE_SUCCESS,
        E_RIDE_PS2_ERR_INVALID_STATE,

        E_RIDE_SUCCESS,
        E_RIDE_PS2_ERR_INVALID_STATE,

        E_RIDE_SUCCESS,
        E_RIDE_PS2_ERR_INVALID_STATE,

        E_RIDE_SUCCESS,
        E_RIDE_PS2_ERR_INVALID_STATE,
    };


    for (int i=0; i<14; i++)
    {
        for (int j=0; j<11; j++)
        {
            e_ride_ps2_add_bit(
                &ps2Pkt,
                testFrames[i][j]
            );
        }

        e_ride_err_t resultCode = e_ride_ps2_check_pkt(&ps2Pkt);

        TEST_ASSERT_EQUAL(expectedErrCode[i], resultCode);
        e_ride_ps2_reset_pkt(&ps2Pkt);
    }
}
