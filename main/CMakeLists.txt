set(COMPONENT_SRCS "main.c" "gsm.c" "ble.c" "led.c" "IP5306.c" "battery.c" "aws/aws.c" "https_ota/httpsota.c" "wifi/wifi_sta.c")
set(COMPONENT_ADD_INCLUDEDIRS "." "aws" "https_ota" "wifi")


register_component()

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
