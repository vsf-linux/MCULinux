/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/


//! \note User Level Board Configuration

#ifndef __VSF_BOARD_CFG_H__
#define __VSF_BOARD_CFG_H__

/*============================ INCLUDES ======================================*/

// retrive compiler information
#define __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/

/*----------------------------------------------------------------------------*
 * Architecture Configurations                                                *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Hal Driver Configurations                                                  *
 *----------------------------------------------------------------------------*/

// Use unused interrupt as SWI
#define VSF_DEV_SWI_NUM                                 2
#define VSF_DEV_SWI0_IRQ                                IRQn_UART14
#define VSF_DEV_SWI1_IRQ                                IRQn_UART15

// actually general hal components will be enabled by default
#ifndef VSF_HAL_USE_RNG
#   define VSF_HAL_USE_RNG                              ENABLED
#endif
#define VSF_HAL_USE_GPIO                                ENABLED
#   define VSF_HW_GPIO_CFG_MULTI_CLASS                  ENABLED
#ifndef VSF_HAL_USE_SPI
#   define VSF_HAL_USE_SPI                              ENABLED
#endif
#ifndef VSF_HAL_USE_MMC
#   define VSF_HAL_USE_MMC                              ENABLED
#endif
#ifndef VSF_HAL_USE_I2S
#   define VSF_HAL_USE_I2S                              ENABLED
#endif
#ifndef VSF_HAL_USE_USART
#   define VSF_HAL_USE_USART                            ENABLED
#endif

/*----------------------------------------------------------------------------*
 * Kernel Configurations                                                      *
 *----------------------------------------------------------------------------*/

// configure pool and heap to avoid heap allocating in interrupt
#define VSF_OS_CFG_EVTQ_POOL_SIZE                       128
#define VSF_OS_CFG_DEFAULT_TASK_FRAME_POOL_SIZE         32
#define VSF_POOL_CFG_FEED_ON_HEAP                       DISABLED

/*----------------------------------------------------------------------------*
 * Components Configurations                                                  *
 *----------------------------------------------------------------------------*/

#define VSF_USE_HEAP                                    ENABLED
#define VSF_HEAP_CFG_MCB_ALIGN_BIT                      4
#   define VSF_HEAP_SIZE                                (128 * 1024)

// debug stream is implemented in vsf_board.c
#define VSF_CFG_DEBUG_STREAM_TX_T                       vsf_stream_t
#define VSF_CFG_DEBUG_STREAM_RX_T                       vsf_mem_stream_t

#define VSF_LINUX_CFG_STACKSIZE                         (8 * 1024)
#define VSF_LINUX_USE_SIMPLE_LIBC                       ENABLED
// HPM environment does not support long double APIs
#define VSF_LINUX_APPLET_LIBC_MATH_LONG_DOULBE          DISABLED
#define VSF_USE_SIMPLE_SPRINTF                          ENABLED
#   define VSF_SIMPLE_SPRINTF_SUPPORT_FLOAT             ENABLED
#define VSF_USE_SIMPLE_SSCANF                           ENABLED

/*----------------------------------------------------------------------------*
 * Application Configurations                                                 *
 *----------------------------------------------------------------------------*/

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __VSF_BOARD_CFG_H__
