#include <esk8_err.h>
#include <esk8_pwm.h>

#include "esk8_pwm_priv.h"

#include <driver/ledc.h>


esk8_err_t
esk8_pwm_sgnl_set(
    esk8_pwm_hndl_t hndl,
    uint8_t pwm_val
)
{
    esk8_pwm_hndl_def_t* pwm_hndl = hndl;

    esp_err_t err;
    err = ledc_set_duty(
        pwm_hndl->speed_mode,
        pwm_hndl->channel,
        pwm_val
    );

    if (err != ESP_OK)
        return ESK8_ERR_INVALID_PARAM;

    err = ledc_update_duty(
        pwm_hndl->speed_mode,
        pwm_hndl->channel
    );

    if (err != ESP_OK)
        return ESK8_ERR_INVALID_PARAM;

    return ESK8_OK;
}