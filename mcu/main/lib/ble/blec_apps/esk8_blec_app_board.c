#include <esk8_blec_apps.h>

static esk8_err_t
app_init(
    uint16_t gattc_if
);

static esk8_err_t
app_deinit(
    uint16_t gattc_if
);

static esk8_err_t
app_conn_add(
    esk8_blec_dev_t* dev,
    uint16_t conn_id
);

static esk8_err_t
app_conn_del(
    esk8_blec_dev_t* dev,
    uint16_t conn_id
);

static esk8_err_t
app_conn_notif(
    esk8_blec_dev_t* dev,
    int attr_idx,
    size_t len,
    uint8_t* val
);

static esk8_err_t
app_conn_read(
    esk8_blec_dev_t* dev,
    int attr_idx,
    size_t len,
    uint8_t* val
);

static esk8_err_t
app_evt_cb(
    esp_gattc_cb_event_t event,
    esp_ble_gattc_cb_param_t *param
);


esp_gattc_db_elem_t db[] = {
    {
    .type               = ESP_GATT_DB_PRIMARY_SERVICE,
    .attribute_handle   = ESK8_BLE_UUID_CTRL,
    .properties = 0,
    .uuid,
    }
}


esk8_blec_app_t
esk8_blec_app_board =
{
    .app_name = "blec_app_board",

    .app_init = app_init,
    .app_deinit = app_deinit,
    .app_conn_add = app_conn_add,
    .app_conn_del = app_conn_del,
    .app_conn_notif = app_conn_notif,
    .app_conn_read = app_conn_read,
    .app_evt_cb = app_evt_cb,

    .ble_elm_n = ,
    .ble_elm_l = 
};


static esk8_err_t
app_init(
    uint16_t gattc_if
)
{
    return ESK8_OK;
}


static esk8_err_t
app_deinit(
    uint16_t gattc_if
)
{
    return ESK8_OK;
}


static esk8_err_t
app_conn_add(
    esk8_blec_dev_t* dev,
    uint16_t conn_id
)
{
    return ESK8_OK;
}


static esk8_err_t
app_conn_del(
    esk8_blec_dev_t* dev,
    uint16_t conn_id
)
{
    return ESK8_OK;
}


static esk8_err_t
app_conn_notif(
    esk8_blec_dev_t* dev,
    int attr_idx,
    size_t len,
    uint8_t* val
)
{
    return ESK8_OK;
}


static esk8_err_t
app_conn_read(
    esk8_blec_dev_t* dev,
    int attr_idx,
    size_t len,
    uint8_t* val
)
{
    return ESK8_OK;
}


static esk8_err_t
app_evt_cb(
    esp_gattc_cb_event_t event,
    esp_ble_gattc_cb_param_t *param
)
{
    return ESK8_OK;
}
