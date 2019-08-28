#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_ble.h>
#include <esk8_bms.h>
#include <esk8_ps2.h>
#include <esk8_pwm.h>
#include <esk8_btn.h>

#include <app_ble_srvc.h>

#include <stdint.h>
#include <stdio.h>

#define LOG_TAG(tag)    "[ " tag " ]: "
#define LOG_TAG_MAIN    LOG_TAG("MAIN")
#define LOG_TAG_PS2     LOG_TAG("PS2")
#define LOG_TAG_BTN     LOG_TAG("BTN")
#define LOG_TAG_BLE     LOG_TAG("BLE")


void status_task(void* param)
{
    esk8_bms_status_t         bmsStatus     = {0};
    esk8_bms_deep_status_t    bmsDeepStatus = {0};
    esk8_bms_config_t*        bmsConfig     = param;

    printf("-----------------------------\n");
    printf("Listening to BMS...          \n");
    printf("-----------------------------\n");

    while(true)
    {
        for (int i=0; i<ESK8_UART_BMS_CONF_NUM; i++)
        {
            esk8_bms_set_rx(bmsConfig, i);
            esk8_err_t errCodeS = esk8_bms_get_status(bmsConfig, &bmsStatus);
            esk8_err_t errCodeDS = esk8_bms_get_deep_status(bmsConfig, &bmsDeepStatus);

            if (errCodeS)
                printf(LOG_TAG_BLE "Got error: %s reading BMS status at index: %d.\n",
                    esk8_err_to_str(errCodeS), i);
            if (errCodeDS)
                printf(LOG_TAG_BLE "Got error: %s reading BMS deep status at index: %d.\n",
                    esk8_err_to_str(errCodeDS), i);

            app_srvc_status_update_bms_shallow(errCodeS, i, &bmsStatus);
            app_srvc_status_update_bms_deep(errCodeDS, i, &bmsDeepStatus);

            /* We might wait quite a long time here */
            vTaskDelay(10000 / portTICK_PERIOD_MS);
        }
    }
}


void ps2_task(void* param)
{
    esk8_ps2_handle_t           ps2Handle;
    esk8_pwm_config_t           pwmCnfg;
    esk8_err_t                  errCode;
    esk8_ps2_mvmnt_t            trckMvmnt;

    errCode = esk8_ps2_init_from_config_h(&ps2Handle);
    if (errCode)
    {
        printf(LOG_TAG_MAIN "Error '%s' on ps2 init\n", esk8_err_to_str(errCode));
        return;
    }

    errCode = esk8_pwm_sgnl_init(&pwmCnfg);
    if (errCode)
        printf(LOG_TAG_MAIN "Error '%s' on pwm init\n", esk8_err_to_str(errCode));

    esk8_ps2_send_cmd(&ps2Handle, ESK8_PS2_CMD_DATA_ENABLE, 100);

    printf("-----------------------------\n");
    printf("Listening to PS2 with PWM... \n");
    printf("-----------------------------\n");

    int speed = 0;
    while(1)
    {
        errCode = esk8_ps2_await_mvmnt(&ps2Handle, &trckMvmnt);
        if (errCode != ESK8_SUCCESS)
        {
            printf(LOG_TAG_PS2 "Got err: %s\n", esk8_err_to_str(errCode));

            /**
             * For good measure let's enable the
             * PS2 device again. It might have disconnected.
             */
            errCode = esk8_ps2_send_cmd(&ps2Handle, ESK8_PS2_CMD_DATA_ENABLE, 100);
            if (errCode)
                printf(LOG_TAG_PS2 "Got err: %s\n", esk8_err_to_str(errCode));

            continue;
        }

        if (trckMvmnt.lftBtn)
            speed = 0;

        speed += trckMvmnt.x;
        speed = speed>255?255:speed;
        speed = speed<  0?  0:speed;

        printf(LOG_TAG_PS2 "Speed: %03d, X: %d %40s\n", speed, trckMvmnt.x, "");

        esk8_pwm_sgnl_set(&pwmCnfg, (uint8_t) speed);
        app_srvc_status_update_speed((uint8_t)speed);
    }
}


void btn_task()
{
    esk8_err_t err_code;
    esk8_btn_cnfg_t btnCnfg = {
        .btn_debounce_ms  = 10,
        .btn_gpio         = GPIO_NUM_0,
        .btn_longPress_ms = 2000,
        .btn_tmrGrp       = 0,
        .btn_tmrIdx       = 1
    };

    err_code = esk8_btn_init(&btnCnfg);
    if (err_code)
    {
        printf(LOG_TAG_MAIN "Error '%s' on btn init\n", esk8_err_to_str(err_code));
        return;
    }

    esk8_btn_press_t btn_press;
    while(1)
    {
        err_code = esk8_btn_await_press(&btnCnfg, &btn_press, 5000);
        if (err_code)
            printf(LOG_TAG_BTN "got err: %d \n", err_code);
        else
            printf(LOG_TAG_BTN "got press: %d \n", btn_press);
    }
}


void app_main()
{
    esk8_err_t  err_code;

    static esk8_ps2_handle_t   ps2Handle;
    static esk8_bms_config_t   bmsConfig;
    static esk8_ble_config_t   bleCnfg;
    static esk8_ble_app_t*     appSrvcList_p[] = APP_ALL_SRVC_LIST_P();

    static esk8_btn_cnfg_t btnCnfg = {
        .btn_debounce_ms  = 10,
        .btn_gpio         = GPIO_NUM_0,
        .btn_longPress_ms = 2000,
        .btn_tmrGrp       = 0,
        .btn_tmrIdx       = 1
    };

    err_code = esk8_ble_init(&bleCnfg);
    if (err_code)
    {
        printf(LOG_TAG_MAIN "Error '%s' on ble init\n", esk8_err_to_str(err_code));
        return;
    }

    err_code = esk8_ble_register_apps((uint16_t) sizeof(appSrvcList_p) / sizeof(appSrvcList_p[0]), appSrvcList_p);
    if (err_code)
    {
        printf(LOG_TAG_MAIN "Error '%s' on ble app register\n", esk8_err_to_str(err_code));
        return;
    }

    /**
     * The BMS is another. It has to start before the button
     * isr and timer. What the hell. If this starts after, it
     * somehow overrides our button / timer isr.
     */
    err_code = esk8_bms_init_from_config_h(&bmsConfig);
    if (err_code)
    {
        printf(LOG_TAG_MAIN "Error '%s' on bms init\n", esk8_err_to_str(err_code));
        return;
    }

    xTaskCreate(ps2_task   , "ps2_task"   , 2048, &ps2Handle, 2, NULL);
    xTaskCreate(status_task, "status_task", 2048, &bmsConfig, 1, NULL);
    xTaskCreate(btn_task   , "btn_task"   , 2048, &btnCnfg  , 1, NULL);

    return;
}
