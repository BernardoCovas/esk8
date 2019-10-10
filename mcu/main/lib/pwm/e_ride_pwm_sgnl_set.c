#include <esk8_err.h>
#include <esk8_pwm.h>

#include <driver/ledc.h>


esk8_err_t
esk8_pwm_sgnl_set(
    esk8_pwm_cnfg_t* cnfg,
    uint8_t pwm_Val
)
{
    esp_err_t err;
    err = ledc_set_duty(
        cnfg->timer_cnfg.speed_mode,
        cnfg->channel_cnfg.channel,
        pwm_Val
    );

    if (err != ESP_OK)
        return ESK8_ERR_INVALID_PARAM;

    err = ledc_update_duty(
        cnfg->timer_cnfg.speed_mode,
        cnfg->channel_cnfg.channel
    );

    if (err != ESP_OK)
        return ESK8_ERR_INVALID_PARAM;

    return ESK8_OK;
}