#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_pwm.h>

#include <driver/ledc.h>
#include <driver/gpio.h>


esk8_err_t esk8_pwm_sgnl_init(
    esk8_pwm_config_t* out_cnfg
)
{
    ledc_timer_config_t t_Config;
    t_Config.duty_resolution    = (ledc_timer_bit_t) ESK8_PWM_NUM_BITS;
    t_Config.freq_hz            = ESK8_PWM_FREQ_HZ;
    t_Config.timer_num          = ESK8_PWM_TIMER_NUM;
    t_Config.speed_mode         = LEDC_HIGH_SPEED_MODE;

    if(ledc_timer_config(&t_Config) != ESK8_SUCCESS)
        return ESK8_ERR_INVALID_PARAM;

    ledc_channel_config_t c_Config;
    c_Config.channel    = ESK8_PWM_CHANNEL;
    c_Config.gpio_num   = ESK8_PWM_GPIO;
    c_Config.intr_type  = LEDC_INTR_DISABLE;
    c_Config.duty       = 0;
    c_Config.speed_mode = LEDC_HIGH_SPEED_MODE; /* HW impl setting changeover. */
    c_Config.timer_sel  = ESK8_PWM_TIMER_NUM;
    c_Config.hpoint     = 0;

    if(ledc_channel_config(&c_Config) != ESP_OK)
        return ESK8_ERR_INVALID_PARAM;

    if (out_cnfg)
    {
        out_cnfg->t_Config = t_Config;
        out_cnfg->c_Config = c_Config;
    }

    return ESK8_SUCCESS;
}

esk8_err_t esk8_pwm_sgnl_stop(
    esk8_pwm_config_t* cnfg
)
{
    ledc_timer_pause(cnfg->t_Config.speed_mode, cnfg->t_Config.timer_num);
    ledc_stop(cnfg->t_Config.speed_mode, cnfg->c_Config.channel, 0);

    return ESK8_SUCCESS;
}