#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_pwm.h>

#include "esk8_pwm_priv.h"

#include <driver/ledc.h>
#include <driver/gpio.h>


esk8_err_t esk8_pwm_sgnl_init(
    esk8_pwm_cnfg_t* cnfg,
    esk8_pwm_hndl_t* out_hndl
)
{
    esk8_pwm_hndl_def_t* hndl = calloc(1, sizeof(esk8_pwm_hndl_def_t));
    if (!hndl)
        return ESK8_ERR_OOM;

    *out_hndl = hndl;

    ledc_timer_config_t tmr_cnfg = {
        .duty_resolution    = 8,
        .freq_hz            = cnfg->pwm_freq_hz,
        .timer_num          = cnfg->timer_num,
        .speed_mode         = LEDC_HIGH_SPEED_MODE
    };

    if(ledc_timer_config(&tmr_cnfg) != ESK8_OK)
    {
        free(*out_hndl);
        return ESK8_ERR_INVALID_PARAM;
    }

    ledc_channel_config_t chnl_cnfg = {
        .channel    = cnfg->pwm_chnl,
        .gpio_num   = cnfg->pwm_gpio,
        .intr_type  = LEDC_INTR_DISABLE,
        .duty       = 0,
        .speed_mode = LEDC_HIGH_SPEED_MODE, /* HW impl setting changeover. */
        .timer_sel  = cnfg->timer_num,
        .hpoint     = 0
    };

    if(ledc_channel_config(&chnl_cnfg) != ESP_OK)
    {
        free(*out_hndl);
        return ESK8_ERR_INVALID_PARAM;
    }

    hndl->channel = cnfg->pwm_chnl;
    hndl->speed_mode = LEDC_HIGH_SPEED_MODE;

    return ESK8_OK;
}


esk8_err_t
esk8_pwm_sgnl_init_from_config_h(
    esk8_pwm_hndl_t* out_hndl
)
{
    esk8_pwm_cnfg_t cnfg;
    cnfg.pwm_chnl = ESK8_PWM_CHANNEL;
    cnfg.pwm_freq_hz = ESK8_PWM_FREQ_HZ;
    cnfg.pwm_gpio = ESK8_PWM_GPIO;
    cnfg.timer_num = ESK8_PWM_TIMER_NUM;

    return esk8_pwm_sgnl_init(&cnfg, out_hndl);
}


esk8_err_t
esk8_pwm_sgnl_stop(
    esk8_pwm_hndl_t hndl
)
{
    esk8_pwm_hndl_def_t* pwm_hndl = hndl;

    ledc_timer_pause(pwm_hndl->speed_mode, pwm_hndl->speed_mode);
    ledc_stop(pwm_hndl->speed_mode, pwm_hndl->channel, 0);

    free(hndl);
    return ESK8_OK;
}