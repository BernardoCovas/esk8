#ifndef _E_RIDE_PWM_H
#define _E_RIDE_PWM_H

#include <e_ride_err.h>

#include <driver/ledc.h>

typedef struct
{
    ledc_timer_config_t t_Config;
    ledc_channel_config_t c_Config;
} e_ride_pwm_config_t;

/**
 * Initializes the pwm signal generator, from
 * the values in e_ride_config.h.
 * If you want any other values, call
 * the pwm native init funcion.
 * Sets `out_cnfg`to the value of the
 * current config.
 **/
e_ride_err_t e_ride_pwm_sgnl_init(

    e_ride_pwm_config_t* out_cnfg

);


/**
 * 
 **/
e_ride_err_t e_ride_pwm_sgnl_set(

    e_ride_pwm_config_t *t_Condig,
    uint8_t pwm_Val

);




#endif /* _E_RIDE_PWM_H */