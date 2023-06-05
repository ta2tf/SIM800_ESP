set(COMPONENT_SRCS "main.c" 
                   "gsm/gsm.c" 
				   "ble/ble.c" 
				   "board/led.c" 
				   "board/IP5306.c" 
				   "board/battery.c" 
				   "board/uart.c" 
				   "aws/aws.c" 
				   "https_ota/httpsota.c" 
				   "wifi/wifi_connect.c" 
				   "callme/whatsapp.c"
				   "ws2812/led_strip_rmt_ws2812.c"
				   )
				   
				   
set(COMPONENT_ADD_INCLUDEDIRS "." "gsm" "ble" "board" "aws" "https_ota" "wifi" "callme" "ws2812")


register_component()

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

