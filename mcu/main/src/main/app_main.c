#include <esk8_config.h>
#include <esk8_log.h>
#include <esk8_err.h>

#ifdef ESK8_REMOTE
#include <esk8_rmt.h>
#include <esk8_blec_apps.h>
#include <esk8_blec/esk8_blec_app_board.h>
#else
#include <esk8_onb.h>
#include <esk8_bles_apps.h>
#endif

#include <stdint.h>
#include <stdio.h>


#ifdef ESK8_REMOTE

static esk8_blec_dev_t
devices[] = {
    {
        .name = "Esk8",
        .addr = { 0x3C, 0x71, 0xBF, 0x4F, 0x0C, 0xA2 }
    }
};


void
app_main()
{

    esk8_err_t err;
    err = esk8_blec_init(1);
    if (err)
        goto fail;

    err = esk8_blec_app_reg(
        &esk8_blec_app_board,
        &devices[0]
    );

    err = esk8_rmt_start();
    if (err)
        goto fail;

    return;

fail:
    esk8_log_E(ESK8_TAG_MAIN,
        "Got err: %s\n",
        esk8_err_to_str(err)
    );
}

#else

void
app_main()
{
    esk8_err_t  err;
    static esk8_bles_app_t* apps[] = {
        &esk8_app_srvc_auth,
        &esk8_app_srvc_ctrl,
        &esk8_app_srvc_status
    };

    err = esk8_bles_apps_init(3, 10);

    if (err)
        esk8_log_E(ESK8_TAG_MAIN,
            "Got %s on ble init\n",
            esk8_err_to_str(err)
        );

    for (int i = 0; i < sizeof(apps) / sizeof(apps[0]); i++)
    {
        err = esk8_bles_app_register(apps[i]);
        if (err)
            esk8_log_E(ESK8_TAG_MAIN,
                "Got %s on ble app '%s'\n",
                esk8_err_to_str(err),
                apps[i]->app_name
            );
    }

    static esk8_onboard_cnfg_t cnfg;
    cnfg.bms_update_ms = 5000;
    cnfg.btn_timeout_ms = 5000; // unused
    cnfg.ps2_timeout_ms = 60000; // unused

    esk8_onboard_start(&cnfg);
    return;
}

#endif /* ESK8_REMOTE */
