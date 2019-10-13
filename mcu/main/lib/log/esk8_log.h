#ifndef _ESK8_LOG_H
#define _ESK8_LOG_H


typedef enum
{
    ESK8_TAG_MAIN,
    ESK8_TAG_PS2,
    ESK8_TAG_BTN,
    ESK8_TAG_BLE,
    ESK8_TAG_NVS,
    ESK8_TAG_ATH,
    ESK8_TAG_ONB,
    ESK8_TAG_RMT,

    ESK8_TAG_MAX
}
esk8_log_tag_t;

typedef enum
{
    ESK8_LOG_LVL_D = 0,
    ESK8_LOG_LVL_I,
    ESK8_LOG_LVL_W,
    ESK8_LOG_LVL_E,

    ESK8_LOG_LVL_MAX
}
esk8_log_level_t;

void
esk8_log(
    esk8_log_level_t lvl,
    esk8_log_tag_t   tag,
    const char*      fmt,
    ...
);

#define esk8_log_D(tag, ...) esk8_log(ESK8_LOG_LVL_D, tag, __VA_ARGS__)
#define esk8_log_I(tag, ...) esk8_log(ESK8_LOG_LVL_I, tag, __VA_ARGS__)
#define esk8_log_W(tag, ...) esk8_log(ESK8_LOG_LVL_W, tag, __VA_ARGS__)
#define esk8_log_E(tag, ...) esk8_log(ESK8_LOG_LVL_E, tag, __VA_ARGS__)


#endif /* _ESK8_LOG_H */
