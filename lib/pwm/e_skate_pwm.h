#ifndef _E_SKATE_PWM_H
#define _E_SKATE_PWM_H

#include <e_skate_err.h>

#include <driver/ledc.h>


/**
 * Initializes the pwm signal generator, from
 * the values in e_skate_config.h.
 * If you want any other values, call
 * the pwm native init funcion.
 * Sets `out_cnfg`to the value of the
 * current config.
 **/
e_skate_err_t e_skate_pwm_sgnl_init(

    ledc_timer_config_t* out_cnfg

);


/**
 * 
 **/
e_skate_err_t e_skate_pwm_sgnl_set(

    uint8_t val

);




#endif /* _E_SKATE_PWM_H */