#include "test_e_skate.h"
#include <e_skate_ps2.h>
#include <e_skate_ps2_utils.h>

#include <unity.h>


void test_e_skate_ps2_check_frame()
{
    
    e_skate_ps2_handle_t ps2Handle;
    e_skate_ps2_reset_pkt(&ps2Handle);

    uint8_t testFrames[][11] =  {
        {/* Start (Should be 1) */ 1, /* Data */ 0, 0, 0, 0, 0, 0, 0, 0, /* Parity (Right) */ 1, /* Stop (Should be 0) */ 0},
        {/* Start (Should be 1) */ 1, /* Data */ 0, 0, 0, 0, 0, 0, 0, 0, /* Parity (Wrong) */ 0, /* Stop (Should be 0) */ 0},

        {/* Start (Should be 1) */ 1, /* Data */ 1, 1, 1, 1, 1, 0, 1, 1, /* Parity (Right) */ 0, /* Stop (Should be 0) */ 0},
        {/* Start (Should be 1) */ 1, /* Data */ 1, 1, 1, 1, 1, 0, 1, 1, /* Parity (Wrong) */ 1, /* Stop (Should be 0) */ 0},
        
        {/* Start (Should be 1) */ 1, /* Data */ 1, 1, 1, 1, 1, 0, 1, 0, /* Parity (Right) */ 1, /* Stop (Should be 0) */ 0},
        {/* Start (Should be 1) */ 1, /* Data */ 1, 1, 1, 1, 1, 0, 1, 0, /* Parity (Wrong) */ 0, /* Stop (Should be 0) */ 0},
        
        {/* Start (Should be 1) */ 1, /* Data */ 1, 1, 1, 1, 1, 1, 1, 1, /* Parity (Right) */ 1, /* Stop (Should be 0) */ 0},
        {/* Start (Should be 1) */ 1, /* Data */ 1, 1, 1, 1, 1, 1, 1, 1, /* Parity (Wrong) */ 0, /* Stop (Should be 0) */ 0},
        
        {/* Start (Should be 1) */ 1, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Right) */ 0, /* Stop (Should be 0) */ 0},
        {/* Start (Should be 1) */ 1, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Wrong) */ 1, /* Stop (Should be 0) */ 0},

        {/* Start (Should be 1) */ 1, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Right) */ 0, /* Stop (Should be 0) */ 0},
        {/* Start (Should be 1) */ 0, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Right) */ 0, /* Stop (Should be 0) */ 0},

        {/* Start (Should be 1) */ 1, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Right) */ 0, /* Stop (Should be 0) */ 0},
        {/* Start (Should be 1) */ 1, /* Data */ 1, 0, 1, 0, 0, 0, 1, 0, /* Parity (Right) */ 0, /* Stop (Should be 0) */ 1},
    };

    esp_err_t expectedErrCode[] = {
        E_SKATE_SUCCESS,
        E_SKATE_PS2_ERR_INVALID_STATE,

        E_SKATE_SUCCESS,
        E_SKATE_PS2_ERR_INVALID_STATE,

        E_SKATE_SUCCESS,
        E_SKATE_PS2_ERR_INVALID_STATE,

        E_SKATE_SUCCESS,
        E_SKATE_PS2_ERR_INVALID_STATE,

        E_SKATE_SUCCESS,
        E_SKATE_PS2_ERR_INVALID_STATE,

        E_SKATE_SUCCESS,
        E_SKATE_PS2_ERR_INVALID_STATE,

        E_SKATE_SUCCESS,
        E_SKATE_PS2_ERR_INVALID_STATE,
    };


    for (int i=0; i<14; i++)
    {
        for (int j=0; j<11; j++)
        {
            e_skate_ps2_add_bit(
                &ps2Handle,
                testFrames[i][j]
            );
        }

        e_skate_err_t resultCode = e_skate_ps2_check_frame(&ps2Handle);

        TEST_ASSERT_EQUAL(expectedErrCode[i], resultCode);
        e_skate_ps2_reset_pkt(&ps2Handle);
    }
}
