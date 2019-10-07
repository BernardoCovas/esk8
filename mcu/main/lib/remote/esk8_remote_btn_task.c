#include <esk8_btn.h>
#include <esk8_remote.h>


void
esk8_remote_task_btn(
    void* param
)
{
    esk8_btn_hndl_t hndl = (esk8_btn_hndl_t)param;
    esk8_btn_press_t press;

    while(1)
    {
        esk8_btn_await_press(
            hndl,
            &press
        );
    }
}