#include <esk8_err.h>
#include <esk8_btn.h>

#include "esk8_btn_priv.h"

#include <esp_err.h>
#include <driver/gpio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#define ESK8_BTN_DEBOUNCE_ms = 10

static void esk8_btn_isr(void* param);
static void esk8_tmr_isr(void* param);


esk8_err_t
esk8_btn_init(
    esk8_btn_hndl_t* out_hndl,
    esk8_btn_cnfg_t* btn_cnfg
)
{
    esp_err_t err;
    esk8_btn_hndl_def_t* btn_hndl = calloc(1, sizeof(esk8_btn_hndl_def_t));
    if (!btn_hndl)
        return ESK8_ERR_OOM;

    btn_hndl->btn_cnfg = (*btn_cnfg);

    gpio_set_direction(btn_cnfg->btn_gpio, GPIO_MODE_INPUT);
    gpio_set_pull_mode(btn_cnfg->btn_gpio, GPIO_PULLUP_ONLY);
    gpio_set_drive_capability(btn_cnfg->btn_gpio, GPIO_DRIVE_CAP_0);
    gpio_set_intr_type(btn_cnfg->btn_gpio, GPIO_INTR_ANYEDGE);
    gpio_intr_disable(btn_cnfg->btn_gpio);

    gpio_install_isr_service(0);

    const esp_timer_create_args_t tmr_args = {
        .name = "btn_long_press",
        .arg = (void*)btn_hndl,
        .callback = esk8_btn_isr,
        .dispatch_method = ESP_TIMER_TASK,
    };

    btn_hndl->que_hndl = xQueueCreate(1, sizeof(esk8_btn_press_t));
    err = esp_timer_create(&tmr_args, &btn_hndl->tmr_hndl);
    if (err)
        btn_hndl->tmr_hndl = NULL;

    if  (
            !btn_hndl->que_hndl ||
            !btn_hndl->tmr_hndl
        )
    {
        esk8_btn_deinit(btn_hndl);
        return ESK8_ERR_OOM;
    }

    gpio_isr_handler_add(
        btn_cnfg->btn_gpio,
        esk8_btn_isr,
        btn_hndl
    );

    return ESK8_OK;
}


esk8_err_t
esk8_btn_await_press(
    esk8_btn_hndl_t*  hndl,
    esk8_btn_press_t* out_press
)
{
    esk8_btn_hndl_def_t* btn_hndl = calloc(1, sizeof(esk8_btn_hndl_def_t));
    esk8_btn_cnfg_t* btn_cnfg = &btn_hndl->btn_cnfg;

    if (!btn_hndl->que_hndl)
        return ESK8_BTN_ERR_NOINIT;

    BaseType_t rcv = xQueueReceive(
        btn_hndl->que_hndl,
        out_press,
        btn_cnfg->timeout_ms / portTICK_PERIOD_MS
    );
    
    if (rcv != pdPASS)
        return ESK8_BTN_ERR_TIMEOUT;

    return ESK8_OK;
}


esk8_err_t
esk8_btn_deinit(
    esk8_btn_hndl_t hndl
)
{
    esk8_btn_hndl_def_t* btn_hndl = (esk8_btn_hndl_def_t*)hndl;
    esk8_btn_cnfg_t* btn_cnfg = &btn_hndl->btn_cnfg;

    if (btn_hndl->que_hndl)
        vQueueDelete(btn_hndl->que_hndl);

    if (btn_hndl->tmr_hndl)
    {
        esp_timer_stop(btn_hndl);
        esp_timer_delete(btn_hndl);
    }

    gpio_isr_handler_remove(btn_cnfg->btn_gpio);
    gpio_set_pull_mode(btn_cnfg->btn_gpio, GPIO_FLOATING);
    gpio_set_direction(btn_cnfg->btn_gpio, GPIO_MODE_DISABLE);

    return ESK8_OK;
}


void esk8_btn_isr(void* param)
{
    esk8_btn_press_t out_prss = ESK8_BTN_PRESS;

    esk8_btn_hndl_def_t* btn_hndl = (esk8_btn_hndl_def_t*)param;
    esk8_btn_cnfg_t* btn_cnfg = &btn_hndl->btn_cnfg;

    int rising = gpio_get_level(btn_cnfg->btn_gpio);
    int64_t timer_us = esp_timer_get_time();
    int64_t delay_us = timer_us - btn_hndl->delay_us;

    if  (
            btn_hndl->hndl_state == ESK8_BTN_STATE_PRESSED &&
            delay_us < btn_cnfg->debounce_ms
        )
        return;

    if (rising)
    {
        switch (btn_hndl->hndl_state)
        {
        case ESK8_BTN_STATE_PRESSED:
            btn_hndl->hndl_state = ESK8_BTN_STATE_RELEASED;
            out_prss = ESK8_BTN_PRESS;
            esp_timer_stop(btn_hndl->tmr_hndl);
            break;

        case ESK8_BTN_STATE_RELEASED:
        default:
            return;
        }
    } else
    {
        switch (btn_hndl->hndl_state)
        {
        case ESK8_BTN_STATE_PRESSED: // timer triggered
            out_prss = ESK8_BTN_LONGPRESS;
            btn_hndl->hndl_state = ESK8_BTN_STATE_RELEASED;
            break;

        case ESK8_BTN_STATE_RELEASED:
            btn_hndl->hndl_state = ESK8_BTN_STATE_PRESSED;
            esp_timer_start_once(
                btn_hndl->tmr_hndl,
                btn_cnfg->longpress_ms * 1000
            );
            return;

        default:
            return;
        }
    }

    xQueueSendFromISR(
        btn_hndl->que_hndl,
        &out_prss,
        NULL
    );
}
