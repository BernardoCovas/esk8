#include <e_skate_err.h>
#include <e_skate_ps2.h>
#include <e_skate_ps2_utils.h>

#include <esp_intr_alloc.h>
#include <driver/gpio.h>


e_skate_err_t e_skate_ps2_deinit(

    e_skate_ps2_handle_t* ps2Handle,
    e_skate_ps2_config_t* ps2Config,
    bool                  withIsr

)
{
    vQueueDelete(ps2Handle->byteQueueHandle);

    ps2Handle->newByte = 0;
    ps2Handle->frameIndex = 0;

    if (withIsr)
        gpio_uninstall_isr_service();
    
    return E_SKATE_SUCCESS;
}