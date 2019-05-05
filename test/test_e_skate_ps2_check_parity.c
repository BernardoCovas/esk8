#include "test_e_skate.h"
#include <e_skate_ps2.h>
#include <e_skate_ps2_utils.h>

#include <unity.h>


void test_e_skate_ps2_check_frame()
{
    
    e_skate_ps2_handle_t ps2Handle;
    e_skate_ps2_reset_handle(
        &ps2Handle
    );

    uint8_t expectedParity[] = {
        1,  // Right
        0,  // Right
        1,  // Right
        1,  // Right
        0,  // Wrong
        1   // Wrong
    };

    esp_err_t expectedErrCode[] = {
        E_SKATE_SUCCESS,
        E_SKATE_SUCCESS,
        E_SKATE_SUCCESS,
        E_SKATE_SUCCESS,
        E_SKATE_PS2_ERR_INVALID_STATE,
        E_SKATE_PS2_ERR_INVALID_STATE
    };

    uint8_t expectedValuesBits[][8] =  {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 0},
        {0, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 1, 0},
    };


    for (int i=0; i<6; i++)
    {
        for (int j=0; j<8; j++)
        {
            e_skate_ps2_add_bit(
                &ps2Handle,
                expectedValuesBits[i][j]
            );
        }

        e_skate_err_t resultCode = e_skate_ps2_check_frame(
            &ps2Handle,
            expectedParity[i]);
        
        TEST_ASSERT_EQUAL(expectedErrCode[i], resultCode);
        e_skate_ps2_reset_handle(&ps2Handle);
    }
}
