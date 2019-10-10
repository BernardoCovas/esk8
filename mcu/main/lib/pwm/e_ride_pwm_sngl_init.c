#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_pwm.h>

#include <driver/ledc.h>
#include <driver/gpio.h>


esk8_err_t esk8_pwm_sgnl_init(
    esk8_pwm_cnfg_t* out_cnfg
)
{
    ledc_timer_config_t tmr_cnfg = {
        .duty_resolution    = (ledc_timer_bit_t) ESK8_PWM_NUM_BITS,
        .freq_hz            = ESK8_PWM_FREQ_HZ,
        .timer_num          = ESK8_PWM_TIMER_NUM,
        .speed_mode         = LEDC_HIGH_SPEED_MODE
    };

    if(ledc_timer_config(&tmr_cnfg) != ESK8_OK)
        return ESK8_ERR_INVALID_PARAM;

    ledc_channel_config_t chnl_cnfg = {
        .channel    = ESK8_PWM_CHANNEL,
        .gpio_num   = ESK8_PWM_GPIO,
        .intr_type  = LEDC_INTR_DISABLE,
        .duty       = 0,
        .speed_mode = LEDC_HIGH_SPEED_MODE, /* HW impl setting changeover. */
        .timer_sel  = ESK8_PWM_TIMER_NUM,
        .hpoint     = 0
    };

    if(ledc_channel_config(&chnl_cnfg) != ESP_OK)
        return ESK8_ERR_INVALID_PARAM;

    out_cnfg->timer_cnfg = tmr_cnfg;
    out_cnfg->channel_cnfg = chnl_cnfg;

    return ESK8_OK;
}

esk8_err_t esk8_pwm_sgnl_stop(
    esk8_pwm_cnfg_t* cnfg
)
{
    ledc_timer_pause(cnfg->timer_cnfg.speed_mode, cnfg->channel_cnfg.timer_sel);
    ledc_stop(cnfg->timer_cnfg.speed_mode, cnfg->channel_cnfg.channel, 0);

    return ESK8_OK;
}