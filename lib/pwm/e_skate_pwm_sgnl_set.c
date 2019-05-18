#include <e_skate_err.h>
#include <e_skate_pwm.h>

#include <driver/ledc.h>


e_skate_err_t e_skate_pwm_sgnl_set(

    e_skate_pwm_config_t* pwm_Config,
    uint8_t pwm_Val

)
{
    ledc_set_duty(pwm_Config->t_Config.speed_mode, pwm_Config->c_Config.channel, pwm_Val);
    ledc_update_duty(pwm_Config->t_Config.speed_mode, pwm_Config->c_Config.channel);

    return E_SKATE_SUCCESS;
}