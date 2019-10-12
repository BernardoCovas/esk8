#include <esk8_config.h>
#include <esk8_log.h>
#include <esk8_err.h>
#include <esk8_bms.h>
#include <esk8_ps2.h>
#include <esk8_pwm.h>
#include <esk8_btn.h>
#include <esk8_auth.h>
#include <esk8_ble_apps.h>
#include <ble_apps/esk8_ble_app_status.h>

#ifdef ESK8_REMOTE
#include <esk8_remote.h>
#else
#include <esk8_onboard.h>
#endif

#include <stdint.h>
#include <stdio.h>


#ifdef ESK8_REMOTE

void
app_main()
{
    esk8_err_t err;
    esk8_remote_cnfg_t rmt_cnfg = { 0 };
    esk8_pwm_cnfg_t pwm_cnfg;

    esk8_btn_cnfg_t btn_cnfg = {
        .btn_gpio = ESK8_BTN_GPIO,
        .debounce_ms = ESK8_BTN_DEBOUNCE_ms,
        .longpress_ms = 1000,
        .timeout_ms = 10000
    };

    esk8_ps2_hndl_t ps2_hndl;
    err = esk8_ps2_init_from_config_h(&ps2_hndl);
    if (err)
        goto fail;

    esk8_btn_hndl_t btn_hndl;
    err = esk8_btn_init(&btn_hndl, &btn_cnfg);
    if (err)
        goto fail;

    err = esk8_pwm_sgnl_init(&pwm_cnfg);
    if (err)
        goto fail;

    err = esk8_remote_start(
        &rmt_cnfg,
        ps2_hndl,
        btn_hndl
    );
    if (err)
        goto fail;

    return;

fail:
    printf(E ESK8_TAG_MAIN
        "Got err: %s\n",
        esk8_err_to_str(err)
    );
}

#else

void
app_main()
{
    esk8_err_t  err_code;

    err_code = esk8_ble_apps_init(3, 10);
    printf(ESK8_TAG_MAIN "Got %s on ble init\n",
        esk8_err_to_str(err_code));
    err_code = esk8_ble_app_register(&esk8_app_srvc_auth);
    printf(ESK8_TAG_MAIN "Got %s on ble app '%s'\n",
        esk8_err_to_str(err_code), esk8_app_srvc_auth.app_name);
    err_code = esk8_ble_app_register(&esk8_app_srvc_ctrl);
    printf(ESK8_TAG_MAIN "Got %s on ble app '%s'\n",
        esk8_err_to_str(err_code), esk8_app_srvc_ctrl.app_name);
    err_code = esk8_ble_app_register(&esk8_app_srvc_status);
    printf(ESK8_TAG_MAIN "Got %s on ble app '%s'\n",
        esk8_err_to_str(err_code), esk8_app_srvc_status.app_name);

    esk8_onboard_cnfg_t ride_cnfg;
    ride_cnfg.bms_update_delay_ms = 5000;
    ride_cnfg.btn_timeout_ms = 5000;

    esk8_onboard_start(&ride_cnfg);
    return;
}

#endif /* ESK8_REMOTE */
