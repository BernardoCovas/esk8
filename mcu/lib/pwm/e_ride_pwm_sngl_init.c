#include <e_ride_config.h>
#include <e_ride_err.h>
#include <e_ride_pwm.h>

#include <driver/ledc.h>
#include <driver/gpio.h>


e_ride_err_t e_ride_pwm_sgnl_init(

    e_ride_pwm_config_t* out_cnfg

)
{
    ledc_timer_config_t t_Config;
    t_Config.duty_resolution    = (ledc_timer_bit_t) E_RIDE_PWM_NUM_BITS;
    t_Config.freq_hz            = E_RIDE_PWM_FREQ_HZ;
    t_Config.timer_num          = E_RIDE_PWM_TIMER_NUM;
    t_Config.speed_mode         = LEDC_HIGH_SPEED_MODE;
 
    if(ledc_timer_config(&t_Config) != E_RIDE_SUCCESS)
        return E_RIDE_ERR_INVALID_PARAM;

    ledc_channel_config_t c_Config;
    c_Config.channel    = E_RIDE_PWM_CHANNEL;
    c_Config.gpio_num   = E_RIDE_PWM_GPIO;
    c_Config.intr_type  = LEDC_INTR_DISABLE;
    c_Config.duty       = 0;
    c_Config.speed_mode = LEDC_HIGH_SPEED_MODE; /* HW impl setting changeover. */
    c_Config.timer_sel  = E_RIDE_PWM_TIMER_NUM;
    c_Config.hpoint     = 0;

    if(ledc_channel_config(&c_Config) != E_RIDE_SUCCESS)
        return E_RIDE_ERR_INVALID_PARAM;

    if (out_cnfg)
    {
        out_cnfg->t_Config = t_Config;
        out_cnfg->c_Config = c_Config;
    }

    return E_RIDE_SUCCESS;
}