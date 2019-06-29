#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_ps2.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>


esk8_err_t esk8_ps2_await_byte(

    esk8_ps2_handle_t*   ps2Handle,
    uint8_t*                outByte,
    int                     timeOut_ms

)
{
    bool received = xQueueReceive(
        ps2Handle->rxByteQueueHandle,
        outByte,
        timeOut_ms / portTICK_PERIOD_MS);

    if (received)
        return ESK8_SUCCESS;

    return ESK8_PS2_ERR_TIMEOUT;
}
