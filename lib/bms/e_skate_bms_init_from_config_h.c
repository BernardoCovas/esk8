#include <e_skate_bms.h>
#include <e_skate_uart_esp.h>
#include <e_skate_uart.h>


e_skate_bms_err_t e_skate_bms_init_from_config_h(

)
{
    uart_config_t bmsConfig = E_SKATE_UART_CONFIG_DEFAULT_ESP32();
    // TODO (bcovas)
    
    return E_SKATE_BMS_SUCCESS;
}