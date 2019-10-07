#include <esk8_config.h>
#include <esk8_onboard.h>
#include <esk8_remote.h>

#include <esk8_log.h>
#include <esk8_err.h>
#include <esk8_bms.h>
#include <esk8_ps2.h>
#include <esk8_pwm.h>
#include <esk8_btn.h>
#include <esk8_auth.h>
#include <esk8_ble_apps.h>
#include <ble_apps/esk8_ble_app_status.h>

#include <stdint.h>
#include <stdio.h>


void app_main_onboard()
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

void app_main_remote()
{
    esk8_remote_start();
}

void app_main()
{

#ifdef ESK8_REMOTE
    app_main_remote();
#else
    app_main_onboard();
#endif

}