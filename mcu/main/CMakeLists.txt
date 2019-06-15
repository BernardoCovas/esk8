file(GLOB_RECURSE _esk8_src "**/*.c")

set(_esk8_include
    "lib/ble"
    "lib/bms"
    "lib/config"
    "lib/error"
    "lib/ps2"
    "lib/pwm"
    "lib/status"
    "lib/track"
    "lib/uart"

    "src/"
    "."
)

set(COMPONENT_SRCS ${_esk8_src})
set(COMPONENT_ADD_INCLUDEDIRS ${_esk8_include})

set(COMPONENT_REQUIRES driver nvs_flash bt)

register_component()
