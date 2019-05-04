#include <e_skate_err.h>
#include <e_skate_config.h>
#include <e_skate_ps2.h>

#include <esp_intr_alloc.h>
#include <driver/gpio.h>


void IRAM_ATTR e_skate_ps2_isr(void** param)
{
    e_skate_ps2_handle_t* ps2Handle = (e_skate_ps2_handle_t*) param[0];
    e_skate_ps2_config_t* ps2Config = (e_skate_ps2_config_t*) param[1];

    bool newBit = gpio_get_level(
        ps2Config->dataPin);

    e_skate_err_t bitErrCode = e_skate_ps2_add_bit(
        ps2Handle,
        newBit);

    if (bitErrCode == E_SKATE_PS2_ERR_VALUE_READY)
    {
        // Send the new byte to the queue
        xQueueSendFromISR(
            ps2Handle->byteQueueHandle,
            &ps2Handle->newByte,
            NULL);

        e_skate_ps2_reset_handle(ps2Handle);
    }
}


e_skate_err_t e_skate_ps2_init(

    e_skate_ps2_handle_t* ps2Handle,
    e_skate_ps2_config_t* ps2Config

)
{
    ps2Handle->byteQueueHandle = xQueueCreate(
        E_SKATE_PS2_BYTE_QUEUE_LENGTH,
        1
    );

    if (ps2Handle->byteQueueHandle == NULL)
        return E_SKATE_ERR_OOM;

    ps2Handle->newByte = 0;
    ps2Handle->newByteIndex = 0;


    // TODO: Set ISR
    return E_SKATE_SUCCESS;
}