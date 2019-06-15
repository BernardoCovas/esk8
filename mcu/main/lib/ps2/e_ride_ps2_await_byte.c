#include <e_ride_config.h>
#include <e_ride_err.h>
#include <e_ride_ps2.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>


e_ride_err_t e_ride_ps2_await_byte(

    e_ride_ps2_handle_t*   ps2Handle,
    uint8_t*                outByte,
    int                     timeOut_ms

)
{
    bool received = xQueueReceive(
        ps2Handle->rxByteQueueHandle,
        outByte,
        timeOut_ms / portTICK_PERIOD_MS);

    if (received)
        return E_RIDE_SUCCESS;

    return E_RIDE_PS2_ERR_TIMEOUT;
}
