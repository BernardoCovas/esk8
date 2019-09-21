#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_ps2.h>
#include "esk8_ps2_utils.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>


esk8_err_t esk8_ps2_await_byte(

    esk8_ps2_handle_t*      ps2Handle,
    uint8_t*                outByte,
    int                     timeOut_ms

)
{
    if (!xQueueReceive(ps2Handle->rxByteQueueHandle, outByte, timeOut_ms / portTICK_PERIOD_MS))
        return ESK8_PS2_ERR_BYTE_TIMEOUT;

    return ESK8_SUCCESS;
}
