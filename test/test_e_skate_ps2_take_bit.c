#include "test_e_skate.h"
#include <e_skate_ps2.h>
#include <e_skate_ps2_utils.h>

#include <unity.h>


void test_e_skate_ps2_take_bit()
{
    e_skate_ps2_pkt_t ps2Pkt;

    ps2Pkt.newStart   = 0;
    ps2Pkt.newStop    = 1;
    ps2Pkt.newByte    = 0xaa;
    ps2Pkt.newParity  = e_skate_ps2_get_parity(ps2Pkt.newByte);
    ps2Pkt.frameIndex = 0;

    bool expctedVals[] = {0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1};

    bool            currBit;
    e_skate_err_t   errCode;

    for (int i=0; i<10; i++)
    {
        errCode = e_skate_ps2_take_bit(&ps2Pkt, &currBit);
        printf("Current bit: %d, idx: %d\n", currBit, ps2Pkt.frameIndex);

        TEST_ASSERT_EQUAL(E_SKATE_SUCCESS, errCode);
        TEST_ASSERT_EQUAL(expctedVals[i], currBit);
    }

    errCode = e_skate_ps2_take_bit(&ps2Pkt, &currBit);
    TEST_ASSERT_EQUAL(E_SKATE_PS2_ERR_VALUE_READY, errCode);
    TEST_ASSERT_EQUAL(expctedVals[10], currBit);

    errCode = e_skate_ps2_take_bit(&ps2Pkt, &currBit);
    TEST_ASSERT_EQUAL(E_SKATE_PS2_ERR_VALUE_READY, errCode);
}