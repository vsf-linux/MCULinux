# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

get_filename_component(CUSTOM_LINKER_FILE vsf.ld ABSOLUTE)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(vsf_template)

# VSF start
if(NOT DEFINED ENV{VSF_PATH})
    get_filename_component(__VSF_PATH ../../vsf ABSOLUTE)
    set(ENV{VSF_PATH} ${__VSF_PATH})
endif()
message(STATUS "VSF_PATH: $ENV{VSF_PATH}")

# board and application settings
set(BOARD_PATH ../../board/hpm6750evk)
set(APPLICATION_PATH ../../application)

# arch/driver
set(VSF_HAL_CHIP_VENDOR     HPM)
set(VSF_HAL_CHIP_SERIES     HPM6700)
set(VSF_HAL_CHIP_NAME       HPM6750)
set(VSF_ARCH_SERIES         rv)
set(VSF_ARCH_NAME           generic)
set(VSF_TARGET_NAME         app)

# modules
set(VSF_USE_LINUX 1)
    set(VSF_LINUX_USE_SIMPLE_LIBC 1)
set(APP_USE_LINUX_BUSYBOX_DEMO 1)

function(vsf_add_sources)
    sdk_app_src(${ARGN})
endfunction()
function(vsf_add_include_directories)
    sdk_app_inc(${ARGN})
endfunction()
function(vsf_add_compile_definitions)
    sdk_compile_definitions(${ARGN})
endfunction()
function(vsf_add_libraries)
    message(FATAL_ERROR "libraries not supported by HPM")
endfunction()

get_filename_component(ABS_BOARD_PATH ${BOARD_PATH} ABSOLUTE)
list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
    ${ABS_BOARD_PATH}
)

vsf_add_compile_definitions(
    __HPM__ __HPM6750__
    __VSF_CPP__
    __VSF__
)
include($ENV{VSF_PATH}/script/cmake/extensions.cmake)
add_subdirectory($ENV{VSF_PATH}/source ${CMAKE_CURRENT_BINARY_DIR}/vsf_bin)
add_subdirectory(${BOARD_PATH} ${CMAKE_CURRENT_BINARY_DIR}/board_bin)
add_subdirectory(${APPLICATION_PATH} ${CMAKE_CURRENT_BINARY_DIR}/application_bin)
vsf_add_sources(
    $ENV{VSF_PATH}/example/template/demo/linux_demo/mount_demo.c
    $ENV{VSF_PATH}/example/template/demo/linux_demo/dynloader_demo.c
)
vsf_add_include_directories(.)

# VSF end

sdk_compile_options(
    "-fms-extensions"
    "-fsigned-char"
)
sdk_compile_definitions(
    -DINIT_EXT_RAM_FOR_DATA=1
    -DBOARD_SHOW_CLOCK=0
    -DBOARD_SHOW_BANNER=0
    -DFLASH_XIP=1
)
sdk_ld_options("-lm")

generate_ses_project()
