set(COMPONENT_SRCS "main.c" "gsm.c" "ble.c" "led.c" "IP5306.c" "battery.c" "aws.c" "httpsota.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")


register_component()

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
