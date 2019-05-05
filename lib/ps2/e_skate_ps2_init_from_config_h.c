#include <e_skate_config.h>
#include <e_skate_err.h>
#include <e_skate_ps2.h>

e_skate_err_t e_skate_ps2_init_from_config_h(

    e_skate_ps2_handle_t* ps2Handle

)
{
    e_skate_ps2_config_t ps2Config= {
        .dataPin  = E_SKATE_PS2_DATA_PIN,
        .clockPin = E_SKATE_PS2_CLOCK_PIN
    };

    return e_skate_ps2_init(
        ps2Handle,
        &ps2Config
    );
}