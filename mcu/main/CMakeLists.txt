file(GLOB_RECURSE _esk8_lib     "lib/**/*.c" "lib/**/*.h")
file(GLOB_RECURSE _esk8_src     "src/main/*.c" "src/main/*.h")
set(_esk8_include "include")

option(ESK8_REMOTE "Build remote control firmware instead of base" OFF)

set(COMPONENT_REQUIRES driver nvs_flash bt)

set(COMPONENT_SRCS ${_esk8_lib} ${_esk8_src})
set(COMPONENT_ADD_INCLUDEDIRS ${_esk8_include})

# Build remote control firmware instead of base
if(ESK8_REMOTE)
    add_definitions(-DESK8_REMOTE)
endif(ESK8_REMOTE)

if (ESK8_LOG_LEVEL)
    add_definitions(-DESK8_LOG_LEVEL=${ESK8_LOG_LEVEL})
endif(ESK8_LOG_LEVEL)


register_component()
