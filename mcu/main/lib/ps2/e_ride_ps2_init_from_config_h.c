#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_ps2.h>

#include <driver/gpio.h>


esk8_err_t
esk8_ps2_init_from_config_h(
    esk8_ps2_hndl_t* hndl
)
{
    esk8_ps2_cnfg_t ps2_config;
    ps2_config.clock_pin = ESK8_PS2_CLOCK_PIN;
    ps2_config.data_pin = ESK8_PS2_DATA_PIN;
    ps2_config.rx_timeout_ms = ESK8_PS2_BYTE_SEND_TIMEOUT_MS;

    return esk8_ps2_init(
        hndl,
        &ps2_config
    );
}
