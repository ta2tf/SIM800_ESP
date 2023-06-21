 
@ECHO off 

 
SET COM=COM17
 
 

ECHO  ---------------------------------------------
ECHO  -- Building Certitificates to store in NVS --
ECHO  ---------------------------------------------

cd .\build\bootloader\
 
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
 0x1000 bootloader.bin
cd..

ECHO  -----------------------
ECHO  -- Flashing Finished --
ECHO  -----------------------
	 
 