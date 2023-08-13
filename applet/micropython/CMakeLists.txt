cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0076 NEW)

# set VSF_PATH and VSF_SRC_PATH
if(NOT DEFINED ENV{VSF_PATH})
    get_filename_component(__VSF_PATH ../../vsf ABSOLUTE)
    set(ENV{VSF_PATH} ${__VSF_PATH})
endif()
message(STATUS "VSF_PATH: $ENV{VSF_PATH}")

# compiler configurations

if(TRUE)
  # arm-none-eabi-gcc will not pass compiler checker of cmake, so include before project
  # **************** Seems not Working ****************
  # refer to arm-none-eabi-gcc BUG: https://answers.launchpad.net/gcc-arm-embedded/+question/675869
  set(CMAKE_C_FLAGS 
      "-Os -fms-extensions -nostartfiles -e _start -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative -fPIC -shared -z max-page-size=4 -nostartfiles -nodefaultlibs -nolibc -nostdlib"
      CACHE INTERNAL "C compiler common flags"
  )
  set(CMAKE_CXX_FLAGS 
      "-Os -fms-extensions -nostartfiles -e _start -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative -fPIC -shared -z max-page-size=4 -nostartfiles -nodefaultlibs -nolibc -nostdlib++"
      CACHE INTERNAL "C++ compiler common flags"
  )
  include($ENV{VSF_PATH}/script/cmake/compilers/gnuarmemb.cmake)
else()
  if(NOT DEFINED LLVM_TOOLCHAIN_PATH)
    message(WARNING "LLVM_TOOLCHAIN_PATH not defined, use default: -DLLVM_TOOLCHAIN_PATH=\"E:/Software/armllvm16\"")
    set(LLVM_TOOLCHAIN_PATH "E:/Software/armllvm16")
  endif()
  if(NOT DEFINED LLVM_TOOLCHAIN_EXE_SUFIX)
    message(WARNING "LLVM_TOOLCHAIN_EXE_SUFIX not defined, use default: -DLLVM_TOOLCHAIN_EXE_SUFIX=\".exe\"")
    set(LLVM_TOOLCHAIN_EXE_SUFIX ".exe")
  endif()
  if(NOT DEFINED LLVM_TOOLCHAIN_SYSROOT)
    message(WARNING "LLVM_TOOLCHAIN_SYSROOT not defined, use default: -DLLVM_TOOLCHAIN_EXE_SUFIX=\"${LLVM_TOOLCHAIN_PATH}/lib/clang-runtimes/arm-none-eabi/armv7m_soft_nofp\"")
    set(LLVM_TOOLCHAIN_SYSROOT "${LLVM_TOOLCHAIN_PATH}/lib/clang-runtimes/arm-none-eabi/armv7m_soft_nofp")
  endif()

  set(CMAKE_C_FLAGS
#      "-Oz -mthumb -fno-builtin-printf -fno-builtin-fprintf -fPIC -mno-pic-data-is-text-relative"      # GOT-base position independency
      "-Oz -mthumb -fno-builtin-printf -fno-builtin-fprintf -fropi -frwpi"                              # embedded position independency
      CACHE INTERNAL "C compiler common flags"
  )
  set(CMAKE_CXX_FLAGS
#      "-Oz -mthumb -fno-builtin-printf -fno-builtin-fprintf -fPIC -mno-pic-data-is-text-relative"
      "-Oz -mthumb -fno-builtin-printf -fno-builtin-fprintf -fropi -frwpi"
      CACHE INTERNAL "C++ compiler common flags"
  )
  include($ENV{VSF_PATH}/script/cmake/compilers/armllvm.cmake)
endif()

project(mpy C CXX ASM)

# target & configurations
set(VSF_TARGET CortexM4)
set(VSF_APPLET 1)
set(VSF_USE_LINUX 1)
    set(VSF_LINUX_USE_SIMPLE_LIBC 1)
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# linker
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    LINKER:-T${CMAKE_CURRENT_SOURCE_DIR}/../linker.ld
    LINKER:-Map=output.map
)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    SUFFIX ".elf"
)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
)

# add user application source code/include directories
vsf_add_compile_definitions(
    __OOC_CPP__         # cpp support in PLOOC
    __CPU_ARM__         # needed in vsf_usr_cfg.h
    __unix__
    MICROPY_PY_TERMIOS=1
    MICROPY_PY_SOCKET=1
    MICROPY_PY_THREAD=1
    MICROPY_PY_THREAD_GIL=0
    MICROPY_PY_SSL=1
    MICROPY_SSL_MBEDTLS=1
)
set(variant minimal)
vsf_add_include_directories(
    ..
    ../include
    # linux include directories
    $ENV{VSF_PATH}/source/shell/sys/linux/include
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc/inttypes
#    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc/math
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc/stdint
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc/assert
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc/setjmp
    $ENV{VSF_PATH}/source/component/3rd-party/mbedtls/raw/include
    ../../application/3rd-party/micropython/port
    ../../application/3rd-party/micropython/port/${variant}
    ../../application/3rd-party/micropython/raw
    ../../application/3rd-party/micropython/raw/ports/unix
    ../../application/3rd-party/micropython/raw/ports/unix/variants/${variant}
)
file(GLOB MPY_CORE_FILES ../../application/3rd-party/micropython/raw/py/*.c)
vsf_add_sources(
    ../../application/3rd-party/micropython/raw/ports/unix/alloc.c
    ../../application/3rd-party/micropython/raw/ports/unix/coverage.c
    ../../application/3rd-party/micropython/raw/ports/unix/gccollect.c
    ../../application/3rd-party/micropython/raw/ports/unix/input.c
    ../../application/3rd-party/micropython/raw/ports/unix/main.c
    ../../application/3rd-party/micropython/raw/ports/unix/modmachine.c
    ../../application/3rd-party/micropython/raw/ports/unix/modtermios.c
    ../../application/3rd-party/micropython/raw/ports/unix/mpthreadport.c
    ../../application/3rd-party/micropython/raw/ports/unix/unix_mphal.c

    ../../application/3rd-party/micropython/raw/extmod/machine_bitstream.c
    ../../application/3rd-party/micropython/raw/extmod/machine_i2c.c
    ../../application/3rd-party/micropython/raw/extmod/machine_mem.c
    ../../application/3rd-party/micropython/raw/extmod/machine_pinbase.c
    ../../application/3rd-party/micropython/raw/extmod/machine_pulse.c
    ../../application/3rd-party/micropython/raw/extmod/machine_pwm.c
    ../../application/3rd-party/micropython/raw/extmod/machine_signal.c
    ../../application/3rd-party/micropython/raw/extmod/machine_spi.c
    ../../application/3rd-party/micropython/raw/extmod/machine_timer.c
    ../../application/3rd-party/micropython/raw/extmod/modasyncio.c
    ../../application/3rd-party/micropython/raw/extmod/modbinascii.c
    ../../application/3rd-party/micropython/raw/extmod/modcryptolib.c
    ../../application/3rd-party/micropython/raw/extmod/moddeflate.c
    ../../application/3rd-party/micropython/raw/extmod/modframebuf.c
    ../../application/3rd-party/micropython/raw/extmod/modhashlib.c
    ../../application/3rd-party/micropython/raw/extmod/modheapq.c
    ../../application/3rd-party/micropython/raw/extmod/modjson.c
    ../../application/3rd-party/micropython/raw/extmod/modnetwork.c
    ../../application/3rd-party/micropython/raw/extmod/modos.c
    ../../application/3rd-party/micropython/raw/extmod/modplatform.c
    ../../application/3rd-party/micropython/raw/extmod/modrandom.c
    ../../application/3rd-party/micropython/raw/extmod/modre.c
    ../../application/3rd-party/micropython/raw/extmod/modselect.c
    ../../application/3rd-party/micropython/raw/extmod/vfs.c
    ../../application/3rd-party/micropython/raw/extmod/vfs_posix.c
    ../../application/3rd-party/micropython/raw/extmod/vfs_posix_file.c
    ../../application/3rd-party/micropython/raw/extmod/vfs_reader.c
    ../../application/3rd-party/micropython/raw/extmod/virtpin.c

    ../../application/3rd-party/micropython/raw/shared/runtime/gchelper_generic.c

    ${MPY_CORE_FILES}
)
#vsf_add_libraries(
#)
#add_subdirectory(
#)
