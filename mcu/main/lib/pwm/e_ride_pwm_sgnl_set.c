#include <esk8_err.h>
#include <esk8_pwm.h>

#include <driver/ledc.h>


esk8_err_t esk8_pwm_sgnl_set(

    esk8_pwm_cnfg_t* pwm_Config,
    uint8_t pwm_Val

)
{
    esp_err_t errCode;
    errCode = ledc_set_duty(pwm_Config->t_Config.speed_mode, pwm_Config->c_Config.channel, pwm_Val);
    if (errCode != ESP_OK)
        return ESK8_ERR_INVALID_PARAM;

    errCode = ledc_update_duty(pwm_Config->t_Config.speed_mode, pwm_Config->c_Config.channel);
    if (errCode != ESP_OK)
        return ESK8_ERR_INVALID_PARAM;

    return ESK8_OK;
}