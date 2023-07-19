set(COMPONENT_SRCS "main.c" 
                   "gsm/gsm.c" 
                   "gsm/libGSM.c" 
				   "ble/ble.c" 
				   "board/led.c" 
				   "board/IP5306.c" 
				   "board/battery.c" 
				   "board/uart.c" 
				   "aws/aws.c" 
				   "https_ota/httpsota.c" 
				   "wifi/wifi_connect.c" 
				   "wifi/sntp.c" 
				   "callme/whatsapp.c"
				   "ws2812/led_strip_rmt_ws2812.c"
				   "http_server/http_server.c"
				   "twai_can/can.c"
				   "twai_can/can_buf.c" 
				   "twai_can/prefilter.c" 
				   "panel/panel.c"			   
				   )
				   
				   
set(COMPONENT_ADD_INCLUDEDIRS "." "gsm" "ble" "board" "aws" "https_ota" "wifi" "callme" "ws2812" "http_server" "twai_can" "panel")


register_component()

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

