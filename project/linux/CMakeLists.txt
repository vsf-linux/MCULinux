cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0076 NEW)

if(NOT DEFINED ENV{VSF_PATH})
    get_filename_component(__VSF_PATH ../../vsf ABSOLUTE)
    set(ENV{VSF_PATH} ${__VSF_PATH})
endif()
message(STATUS "VSF_PATH: $ENV{VSF_PATH}")

# board and application settings
set(BOARD_PATH ../../board/linux)
set(APPLICATION_PATH ../../application)

project(vsf_template)
set(VSF_TARGET generic/linux64)

# compiler configurations
# if compiler is not set, cmake will find one

# VSF_USE configurations
set(VSF_USE_LINUX 1)
    set(VSF_LINUX_USE_SIMPLE_LIBC 1)
set(VSF_USE_DISP 1)
    set(VSF_DISP_USE_SDL2 1)
set(VSF_USE_MBEDTLS 0)

# include vsf cmake script
set(VSF_CONFIG_PATH ${APPLICATION_PATH})
get_filename_component(ABS_BOARD_PATH ${BOARD_PATH} ABSOLUTE)
list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
    ${ABS_BOARD_PATH}
)
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# add user application source code/include directories
# add template directory
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${APPLICATION_PATH}/3rd-party/alsa/port
    ${APPLICATION_PATH}/3rd-party/alsa/port/include
    ${APPLICATION_PATH}/3rd-party/alsa/alsa-utils/include
    ${APPLICATION_PATH}/3rd-party/alsa/alsa-lib/include
)
add_subdirectory(${BOARD_PATH} ${CMAKE_CURRENT_BINARY_DIR}/board_bin)
add_subdirectory(${APPLICATION_PATH} ${CMAKE_CURRENT_BINARY_DIR}/application_bin)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    $ENV{VSF_PATH}/example/template/demo/linux_demo/libusb_demo.c
    $ENV{VSF_PATH}/example/template/demo/linux_demo/mount_demo.c
    $ENV{VSF_PATH}/example/template/demo/linux_demo/ntpdate_demo.c
    $ENV{VSF_PATH}/example/template/demo/linux_demo/telnetd_demo.c
    $ENV{VSF_PATH}/example/template/demo/linux_demo/dynloader_demo.c
)
