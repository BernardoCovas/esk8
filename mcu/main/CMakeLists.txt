file(GLOB_RECURSE _esk8_lib     "lib/**/*.c")
file(GLOB_RECURSE _esk8_src     "src/*.c")
file(GLOB_RECURSE _esk8_rmt     "rmt/*.c")

option(ESK8_BUILD_REMOTE "Build remote control firmware instead of base" OFF)

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

# Build remote control firmware instead of base
if(ESK8_BUILD_REMOTE)

    set(COMPONENT_REQUIRES driver nvs_flash bt)
    set(COMPONENT_SRCS ${_esk8_lib} ${_esk8_rmt})
    set(COMPONENT_ADD_INCLUDEDIRS ${_esk8_include})

else()

    set(COMPONENT_REQUIRES driver nvs_flash bt)
    set(COMPONENT_SRCS ${_esk8_lib} ${_esk8_src})
    set(COMPONENT_ADD_INCLUDEDIRS ${_esk8_include})

endif(ESK8_BUILD_REMOTE)


register_component()
