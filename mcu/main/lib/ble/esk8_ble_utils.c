#include <esk8_err.h>
#include <esk8_ble.h>


esk8_err_t esk8_ble_get_idx_from_handle(

    esk8_ble_app_t* app,
    uint16_t        handle,
    int*            out_idx

)
{
    for (int i=0; i<app->attr_numAttr; i++)
        if (app->attr_hndlList[i] == handle)
        {
            (*out_idx) = i;
            return ESK8_SUCCESS;
        }

    return ESK8_ERR_INVALID_PARAM;
}
