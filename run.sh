#!/bin/bash

SERIAL_PORT="/dev/cu.usbserial-1410"

echo "--> Building regular firmware"
~/.espressif/python_env/idf4.3_py2.7_env/bin/python \ 
    ~/esp/esp-idf/tools/idf.py build

echo "--> Creating NVS partition bin file with AWS IoT certificates"
cd build

~/esp/esp-idf/components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py generate "../main/nvs.csv" certs.bin 12288

echo "--> Flashing to board..."
~/.espressif/python_env/idf4.3_py2.7_env/bin/python \
    ~/esp/esp-idf/components/esptool_py/esptool/esptool.py \
    -p $SERIAL_PORT \
    -b 460800 \
    --before default_reset \
    --after hard_reset \
    --chip esp32 \
    write_flash --flash_mode dio \
    --flash_freq 40m \
    --flash_size detect \
    0x10000 src-firmware-cam-idf.bin \
    0x1000 bootloader/bootloader.bin \
    0x8000 partition_table/partition-table.bin \
    0x9000 certs.bin

cd ..

echo "--> Starting monitor..."
~/.espressif/python_env/idf4.3_py2.7_env/bin/python \
    ~/esp/esp-idf/tools/idf.py \
    -p $SERIAL_PORT monitor