 
@ECHO off 

ECHO  Building regular firmware
idf.py build

ECHO  -- Building Certitificates to store in NVS
cd .\build\
python C:\Espressif\frameworks\esp-idf-v5.0.1\components\nvs_flash\nvs_partition_generator\nvs_partition_gen.py generate ^
"C:\Espressif\frameworks\esp-idf-v5.0.1\workspace2\SIM800_ESP\nvs.csv" certs.bin 16384

ECHO  -- Flashing to board --
python C:\Espressif\frameworks\esp-idf-v5.0.1\components\esptool_py\esptool\esptool.py ^
 -p COM17 ^
 -b 460800 ^
 --before default_reset ^
 --after hard_reset  ^
 --chip esp32 write_flash ^
 --flash_mode dio  ^
 --flash_freq 40m  ^
 --flash_size detect  ^
 0x10000 gatt_server_service_table_demo.bin  ^
 0x1000 bootloader\bootloader.bin  ^
 0x8000 partition_table\partition-table.bin ^
 0x9000 certs.bin
cd..

ECHO  -- Flashing Finished --

	 