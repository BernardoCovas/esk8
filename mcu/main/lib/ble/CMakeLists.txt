file(GLOB COMPONENT_SRCS "*.c")

set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_REQUIRES bt)

register_component()
