# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

set(PROJECT_VER "01.00.00.07")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(gatt_server_service_table_demo)


target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/aws/client.crt" TEXT)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/aws/client.key" TEXT)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/aws/awsrootca.crt" TEXT)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/https_ota/google.crt" TEXT)

 