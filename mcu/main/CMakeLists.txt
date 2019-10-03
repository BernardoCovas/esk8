file(GLOB_RECURSE _esk8_lib     "lib/**/*.c" "lib/**/*.h")
file(GLOB_RECURSE _esk8_src     "src/main/*.c" "src/main/*.h")

option(ESK8_REMOTE "Build remote control firmware instead of base" OFF)

set(_esk8_include
    "lib/auth"
    "lib/ble"
    "lib/bms"
    "lib/btn"
    "lib/config"
    "lib/err"
    "lib/log"
    "lib/nvs"
    "lib/onboard"
    "lib/remote"
    "lib/ps2"
    "lib/pwm"
    "lib/uart"
)

set(COMPONENT_REQUIRES driver nvs_flash bt)

set(COMPONENT_SRCS ${_esk8_lib} ${_esk8_src})
set(COMPONENT_ADD_INCLUDEDIRS ${_esk8_include})

# Build remote control firmware instead of base
if(ESK8_REMOTE)
    add_definitions(ESK8_REMOTE)
endif(ESK8_REMOTE)


register_component()
