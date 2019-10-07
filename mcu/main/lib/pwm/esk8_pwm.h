#ifndef _ESK8_PWM_H
#define _ESK8_PWM_H

#include <esk8_err.h>

#include <driver/ledc.h>

typedef struct
{
    ledc_timer_config_t t_Config;
    ledc_channel_config_t c_Config;
} esk8_pwm_cnfg_t;

/**
 * Initializes the pwm signal generator, from
 * the values in esk8_config.h.
 * If you want any other values, call
 * the pwm native init funcion.
 * Sets `out_cnfg`to the value of the
 * current config.
 **/
esk8_err_t esk8_pwm_sgnl_init(
    esk8_pwm_cnfg_t* out_cnfg
);

/**
 *
 **/
esk8_err_t esk8_pwm_sgnl_set(
    esk8_pwm_cnfg_t *t_Condig,
    uint8_t pwm_Val
);

/**
 *
 */
esk8_err_t esk8_pwm_sgnl_stop(
    esk8_pwm_cnfg_t* cnfg
);


#endif /* _ESK8_PWM_H */