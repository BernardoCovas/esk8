#include <e_skate_err.h>
#include <e_skate_config.h>
#include <e_skate_ps2.h>

#include <esp_intr_alloc.h>
#include <driver/gpio.h>


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

    return E_SKATE_SUCCESS;
}