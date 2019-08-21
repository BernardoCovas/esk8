#include <esk8_err.h>
#include <esk8_btn.h>

#include <esp_err.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <driver/gpio.h>


#define ESK8_BTN_DEBOUNCE_ms = 10


static void _esk8_btn_isr  (void* param);
static void _esk8_tmr_isr  (void* param);


esk8_err_t esk8_btn_init(

    esk8_btn_cnfg_t* btnCnfg

)
{

    btnCnfg->_que_hndlr     = NULL;
    btnCnfg->_tsk_hndlr     = NULL;
    btnCnfg->_hndlr_dir = 0;
    btnCnfg->_hndlr_ms  = 0;

    const timer_config_t t_Config = {
        .alarm_en    = false,
        .auto_reload = true,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en  = false,
        .divider     = 80,
        .intr_type   = TIMER_INTR_LEVEL
    };

#define __esk8_err_handle(X) ESK8_ERRCHECK_DO(X, { esk8_btn_clear(btnCnfg); return ESK8_ERR_INVALID_PARAM; });

    __esk8_err_handle(gpio_set_direction        (btnCnfg->btn_gpio, GPIO_MODE_INPUT   ));
    __esk8_err_handle(gpio_set_pull_mode        (btnCnfg->btn_gpio, GPIO_PULLUP_ONLY  ));
    __esk8_err_handle(gpio_set_drive_capability (btnCnfg->btn_gpio, GPIO_DRIVE_CAP_0  ));
    __esk8_err_handle(gpio_set_intr_type        (btnCnfg->btn_gpio, GPIO_INTR_ANYEDGE ));
    __esk8_err_handle(gpio_intr_disable         (btnCnfg->btn_gpio                    ));

    gpio_install_isr_service(0);

    btnCnfg->_que_hndlr = xQueueCreate(1, sizeof(esk8_btn_press_t));
    if (!btnCnfg->_que_hndlr) { esk8_btn_clear(btnCnfg); return ESK8_ERR_OOM; }

    __esk8_err_handle(gpio_isr_handler_add(btnCnfg->btn_gpio, _esk8_btn_isr, btnCnfg));

    timer_group_t tg = btnCnfg->btn_tmrGrp;
    timer_idx_t   ti = btnCnfg->btn_tmrIdx;

    __esk8_err_handle(timer_init(tg, ti, &t_Config));

    __esk8_err_handle(timer_isr_register(tg, ti, _esk8_tmr_isr, (void*)btnCnfg, 0, NULL));
    __esk8_err_handle(timer_enable_intr(tg, ti));
    __esk8_err_handle(timer_set_alarm_value(tg, ti, 1000 * btnCnfg->btn_longPress_ms));

    return ESK8_SUCCESS;
}


esk8_err_t esk8_btn_await_press(

    esk8_btn_cnfg_t*    btnCnfg,
    esk8_btn_press_t*   out_pressType,
    uint32_t            timeOut_ms

)
{
    if (!btnCnfg->_que_hndlr)
        return ESK8_BTN_ERR_NOINIT;

    if (!xQueueReceive(btnCnfg->_que_hndlr, out_pressType, timeOut_ms / portTICK_PERIOD_MS))
    {
        return ESK8_BTN_ERR_TIMEOUT;
    }

    return ESK8_SUCCESS;
}


esk8_err_t esk8_btn_clear(

    esk8_btn_cnfg_t* btnCnfg

)
{
    if (btnCnfg->_que_hndlr)
        vQueueDelete(btnCnfg->_que_hndlr);

    if (btnCnfg->_tsk_hndlr)
        vTaskDelete(btnCnfg->_tsk_hndlr);

    if (btnCnfg->_smp_hndlr)
        vTaskNotifyGiveFromISR(btnCnfg->_tsk_hndlr, NULL);

    timer_group_t tg = btnCnfg->btn_tmrGrp;
    timer_idx_t   ti = btnCnfg->btn_tmrIdx;

    gpio_isr_handler_remove(btnCnfg->btn_gpio);
    gpio_set_pull_mode(btnCnfg->btn_gpio, GPIO_FLOATING);
    gpio_set_direction(btnCnfg->btn_gpio, GPIO_MODE_DISABLE);
    timer_pause(tg, ti);
    timer_disable_intr(tg, ti);

    return ESK8_SUCCESS;
}


void _esk8_tmr_isr(void* param)
{
    esk8_btn_press_t out_pres = ESK8_BTN_LONGPRESS;
    esk8_btn_cnfg_t*   btnCnfg = (esk8_btn_cnfg_t*)param;

    if (btnCnfg->_que_hndlr)
        /* TODO: Issues if this fails */
        xQueueSendFromISR(btnCnfg->_que_hndlr, &out_pres, NULL);

    TIMERG0.int_clr_timers.t1 = 1;
}


void _esk8_btn_isr(void* param)
{
    esk8_btn_press_t out_btnPress = ESK8_BTN_PRESS;

    esk8_btn_cnfg_t*   btnCnfg = (esk8_btn_cnfg_t*)param;
    timer_group_t tg = btnCnfg->btn_tmrGrp;
    timer_idx_t   ti = btnCnfg->btn_tmrIdx;

    bool level;
    double timer_ms;


    level = (bool)gpio_get_level(btnCnfg->btn_gpio);
    timer_get_counter_time_sec(tg, ti, &timer_ms);
    timer_ms = timer_ms * 1000;


    if (timer_ms == 0)
    {
       timer_set_alarm(tg, ti, TIMER_ALARM_EN);
       timer_start(tg, ti);
       return;
    }


    if (timer_ms >= btnCnfg->btn_longPress_ms)
    {
        timer_pause(ti, tg);
        timer_set_counter_value(tg, ti, 0);
    }

    if (timer_ms == 0 || !level)
    {
       timer_set_alarm(tg, ti, TIMER_ALARM_EN);
       timer_start(tg, ti);
       return;
    }

    if (timer_ms < btnCnfg->btn_debounce_ms) // Debounce
        return;

    /** 
     * The button was released after the debounce,
     * but before the timer
     */
    timer_pause(tg, ti);
    timer_set_counter_value(tg, ti, 0);
    xQueueSendFromISR(btnCnfg->_que_hndlr, &out_btnPress, NULL);
}
