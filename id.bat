 
@ECHO off 

Echo this cript create nvs.csv file with arg device id  and build certs.bin from NVS.CSV and flash 0x9000

  del nvs.csv

 ECHO key,type,encoding,value>nvs.csv
 ECHO certs,namespace,,>>nvs.csv
 ECHO certificate,file,string,../main/aws/awsrootca.crt>>nvs.csv
 ECHO priv_crt,file,string,../main/aws/client.crt>>nvs.csv
 ECHO priv_key,file,string,../main/aws/client.key>>nvs.csv
 ECHO dev_id,data,string,%1>>nvs.csv
 
 
  
SET COM=COM17
 
 

ECHO  ---------------------------------------------
ECHO  -- Building Certitificates to store in NVS --
ECHO  ---------------------------------------------

cd .\build\
python C:\Espressif\frameworks\esp-idf-v5.0.1\components\nvs_flash\nvs_partition_generator\nvs_partition_gen.py generate ^
"C:\Espressif\frameworks\esp-idf-v5.0.1\workspace2\SIM800_ESP\nvs.csv" certs.bin 16384

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
 0x9000 certs.bin
cd..

ECHO  -----------------------
ECHO  -- Flashing Finished --
ECHO  -----------------------
	 
 