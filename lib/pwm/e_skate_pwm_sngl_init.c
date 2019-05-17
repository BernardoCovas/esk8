#include <e_skate_config.h>
#include <e_skate_err.h>
#include <e_skate_pwm.h>

#include <driver/ledc.h>


e_skate_err_t e_skate_pwm_sgnl_init(

    ledc_timer_config_t* out_cnfg

)
{
    esp_err_t errCode;

    ledc_timer_config_t t_Config;
    t_Config.duty_resolution = (ledc_timer_bit_t) E_SKATE_PWM_NUM_BITS;
    t_Config.freq_hz = E_SKATE_PWM_FREQ_HZ;
    t_Config.timer_num = E_SKATE_PWM_TIMER_NUM;

    if(ledc_timer_config(&t_Config) != E_SKATE_SUCCESS)
        return E_SKATE_ERR_INVALID_PARAM;
    
    ledc_channel_config_t c_Config;
    c_Config.channel    = E_SKATE_PWM_CHANNEL;
    c_Config.gpio_num   = E_SKATE_PWM_GPIO;
    c_Config.intr_type  = LEDC_INTR_DISABLE;
    c_Config.duty       = 0;
    c_Config.speed_mode = LEDC_HIGH_SPEED_MODE; /* HW impl setting changeover. */
    c_Config.timer_sel  = E_SKATE_PWM_TIMER_NUM;
    c_Config.hpoint     = 0xfffff;

    if(ledc_channel_config(&c_Config) != E_SKATE_SUCCESS)
        return E_SKATE_ERR_INVALID_PARAM;

    if (out_cnfg)
        (*out_cnfg) = t_Config;

    return E_SKATE_SUCCESS;
}