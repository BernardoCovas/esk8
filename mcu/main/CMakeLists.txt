file(GLOB_RECURSE _esk8_lib     "lib/**/*.c" "lib/**/*.h")
file(GLOB_RECURSE _esk8_src     "src/main/*.c" "src/main/*.h")
file(GLOB_RECURSE _esk8_rmt     "src/ctrl/*.c" "src/ctrl/*.h")

option(ESK8_BUILD_REMOTE "Build remote control firmware instead of base" OFF)

set(_esk8_include
    "lib/ble"
    "lib/bms"
    "lib/btn"
    "lib/config"
    "lib/error"
    "lib/nvs"
    "lib/ps2"
    "lib/pwm"
    "lib/uart"

    "src/include"
    "."
)

set(COMPONENT_REQUIRES driver nvs_flash bt)

# Build remote control firmware instead of base
if(ESK8_BUILD_REMOTE)

    set(COMPONENT_SRCS ${_esk8_lib} ${_esk8_rmt})
    set(COMPONENT_ADD_INCLUDEDIRS ${_esk8_include})

else()

    set(COMPONENT_SRCS ${_esk8_lib} ${_esk8_src})
    set(COMPONENT_ADD_INCLUDEDIRS ${_esk8_include})

endif(ESK8_BUILD_REMOTE)


register_component()
