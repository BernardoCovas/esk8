#include <e_ride_config.h>
#include <e_ride_err.h>
#include <e_ride_ps2.h>

e_ride_err_t e_ride_ps2_init_from_config_h(

    e_ride_ps2_handle_t* ps2Handle

)
{
    e_ride_ps2_config_t ps2Config= {
        .timerConfig = {
            .timerIdx       = E_RIDE_PS2_TIMER_IDX,
            .timerGroup     = E_RIDE_PS2_TIMER_GROUP
        },
        .gpioConfig = {
            .dataPin  = E_RIDE_PS2_DATA_PIN,
            .clockPin = E_RIDE_PS2_CLOCK_PIN
        }
    };
    

    return e_ride_ps2_init(
        ps2Handle,
        &ps2Config
    );
}