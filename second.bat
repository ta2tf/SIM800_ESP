 
@ECHO off 

Echo this script build second.bin from storage.CSV and flash 0x110000

SET COM=COM55
 
 

ECHO  ---------------------------------------------
ECHO  -- Building   to store in NVS --
ECHO  ---------------------------------------------

cd .\build\
python C:\Espressif\frameworks\esp-idf-v5.0.1-3\components\nvs_flash\nvs_partition_generator\nvs_partition_gen.py generate ^
"C:\Espressif\frameworks\esp-idf-v5.0.1-3\workspace\SIM800_ESP\storage.csv" second.bin 16384

 

ECHO  --------------------
ECHO  -- Flashing Start --
ECHO  --------------------

python C:\Espressif\frameworks\esp-idf-v5.0.1\components\esptool_py\esptool\esptool.py ^
 -p %COM% ^
 -b 921600  ^
 --before default_reset ^
 --after hard_reset  ^
 --chip esp32 write_flash ^
 --flash_mode dio  ^
 --flash_freq 40m  ^
 --flash_size detect  ^
 0x110000 second.bin 
 
cd..

ECHO  -----------------------
ECHO  -- Flashing Finished --
ECHO  -----------------------
	 