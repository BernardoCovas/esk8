#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_ps2.h>

#include <driver/gpio.h>


esk8_err_t esk8_ps2_init_from_config_h(

    esk8_ps2_handle_t* ps2Handle

)
{
    esk8_ps2_config_t ps2Config= {
        .timerConfig = {
            .timerIdx       = ESK8_PS2_TIMER_IDX,
            .timerGroup     = ESK8_PS2_TIMER_GROUP
        },
        .gpioConfig = {
            .dataPin  = ESK8_PS2_DATA_PIN,
            .clockPin = ESK8_PS2_CLOCK_PIN
        }
    };
    

    return esk8_ps2_init(
        ps2Handle,
        &ps2Config
    );
}