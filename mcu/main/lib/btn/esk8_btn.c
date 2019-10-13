#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_btn.h>

#include "esk8_btn_priv.h"

#include <esp_err.h>
#include <driver/gpio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <freertos/task.h>

static void IRAM_ATTR esk8_btn_isr(void* param);
static void           esk8_tmr_isr(void* param);


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

    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);

    const esp_timer_create_args_t tmr_args = {
        .name = "btn_long_press",
        .arg = (void*)btn_hndl,
        .callback = esk8_tmr_isr,
        .dispatch_method = ESP_TIMER_TASK,
    };

    btn_hndl->que_hndl = xQueueCreate(1, sizeof(esk8_btn_press_t));
    err = esp_timer_create(
        &tmr_args,
        (esp_timer_handle_t*)&btn_hndl->tmr_hndl
    );
 
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

    (*out_hndl) = btn_hndl;
    return ESK8_OK;
}


esk8_err_t
esk8_btn_init_from_config_h(
    esk8_btn_hndl_t* out_hndl
)
{
    esk8_btn_cnfg_t cnfg;
    cnfg.btn_gpio = ESK8_BTN_GPIO;
    cnfg.debounce_ms = ESK8_BTN_DEBOUNCE_ms;
    cnfg.longpress_ms = ESK8_BTN_LONGPRESS_ms;
    cnfg.timeout_ms = ESK8_BTN_TIMEOUT_ms;

    return esk8_btn_init(out_hndl, &cnfg);
}


esk8_err_t
esk8_btn_await_press(
    esk8_btn_hndl_t   hndl,
    esk8_btn_press_t* out_press
)
{
    esk8_btn_hndl_def_t* btn_hndl = (esk8_btn_hndl_def_t*)hndl;
    esk8_btn_cnfg_t* btn_cnfg = &btn_hndl->btn_cnfg;

    if (!btn_hndl->que_hndl)
        return ESK8_ERR_BTN_NOINIT;

    BaseType_t rcv = xQueueReceive(
        btn_hndl->que_hndl,
        out_press,
        btn_cnfg->timeout_ms / portTICK_PERIOD_MS
    );

    if (rcv != pdPASS)
        return ESK8_ERR_BTN_TIMEOUT;

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
        esp_timer_stop(btn_hndl->tmr_hndl);
        esp_timer_delete(btn_hndl->tmr_hndl);
    }

    gpio_isr_handler_remove(btn_cnfg->btn_gpio);
    gpio_set_pull_mode(btn_cnfg->btn_gpio, GPIO_FLOATING);
    gpio_set_direction(btn_cnfg->btn_gpio, GPIO_MODE_DISABLE);

    return ESK8_OK;
}


void
esk8_tmr_isr(
    void* param
)
{
    esk8_btn_press_t out_prss = ESK8_BTN_LONGPRESS;

    esk8_btn_hndl_def_t* btn_hndl = (esk8_btn_hndl_def_t*)param;
    btn_hndl->hndl_state = ESK8_BTN_STATE_RELEASED;

    xQueueSendFromISR(
        btn_hndl->que_hndl,
        &out_prss,
        NULL
    );
}


void
esk8_btn_isr(
    void* param
)
{
    esk8_btn_press_t out_prss;
    esk8_btn_hndl_def_t* btn_hndl = (esk8_btn_hndl_def_t*)param;
    esk8_btn_cnfg_t* btn_cnfg = &btn_hndl->btn_cnfg;

    int btn_up = gpio_get_level(btn_cnfg->btn_gpio);
    int64_t timer_us = esp_timer_get_time();
    int64_t delay_us = timer_us - btn_hndl->delay_us;

    if  (delay_us < (btn_cnfg->debounce_ms * 1000))
        return;
    btn_hndl->delay_us = timer_us;

    if (btn_up)
    {
        switch (btn_hndl->hndl_state)
        {
        case ESK8_BTN_STATE_PRESSED:
            esp_timer_stop(btn_hndl->tmr_hndl);
            btn_hndl->hndl_state = ESK8_BTN_STATE_RELEASED;
            out_prss = ESK8_BTN_PRESS;
            break;

        case ESK8_BTN_STATE_RELEASED: // Timer already triggered
        default:
            return;
        }
    } else // btn_down
    {
        switch (btn_hndl->hndl_state)
        {
        case ESK8_BTN_STATE_RELEASED: // button pressed, no timer
            btn_hndl->hndl_state = ESK8_BTN_STATE_PRESSED;
            esp_timer_start_once(
                btn_hndl->tmr_hndl,
                btn_cnfg->longpress_ms * 1000
            );
            return;

        case ESK8_BTN_STATE_PRESSED:
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
