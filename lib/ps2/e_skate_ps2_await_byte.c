#include <e_skate_config.h>
#include <e_skate_err.h>
#include "e_skate_ps2.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>


e_skate_err_t e_skate_ps2_await_byte(

    e_skate_ps2_handle_t* ps2Handle,
    uint8_t*              outByte

)
{
    bool received = xQueueReceive(
        ps2Handle->rxByteQueueHandle,
        outByte,
        E_SKATE_PS2_BYTE_QUEUE_TIMEOUT_MS / portTICK_PERIOD_MS);

    if (received)
        return E_SKATE_SUCCESS;

    return E_SKATE_PS2_ERR_TIMEOUT;
}
