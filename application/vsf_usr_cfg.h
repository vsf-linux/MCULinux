/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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


//! \note User Level Application Configuration

#ifndef __VSF_USR_CFG_H__
#define __VSF_USR_CFG_H__

/*============================ INCLUDES ======================================*/

#include "vsf_board_cfg.h"

/*============================ MACROS ========================================*/

// components
#define VSF_USE_HASH                                    ENABLED
#   define VSF_HASH_USE_CRC                             ENABLED
#define VSF_USE_TRACE                                   ENABLED
#define VSF_USE_FIFO                                    ENABLED
#define VSF_USE_JSON                                    ENABLED
#define VSF_USE_DISTBUS                                 ENABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED
#define VSF_USE_STREAM                                  DISABLED
#define VSF_USE_LOADER                                  ENABLED
#   define VSF_LOADER_USE_ELF                           ENABLED
//#   define VSF_ELFLOADER_CFG_DEBUG                      ENABLED
#   ifdef __WIN__
#       define VSF_LOADER_USE_PE                        ENABLED
//#       define VSF_PELOADER_CFG_DEBUG                   ENABLED
#   endif

#define VSF_USE_MAL                                     ENABLED
#   define VSF_MAL_USE_FILE_MAL                         ENABLED
#   define VSF_MAL_USE_SCSI_MAL                         ENABLED
#define VSF_USE_FS                                      ENABLED
#   define VSF_FS_USE_FATFS                             ENABLED
#define VSF_USE_INPUT                                   ENABLED
#define VSF_USE_SCSI                                    ENABLED
#   define VSF_SCSI_USE_MAL_SCSI                        ENABLED
#define VSF_USE_USB_HOST                                ENABLED
#   define VSF_USBH_USE_LIBUSB                          ENABLED
#   define VSF_USBH_USE_MSC                             ENABLED
#   define VSF_USBH_USE_HID                             ENABLED
#   define VSF_USBH_USE_DS4                             ENABLED
#   define VSF_USBH_USE_NSPRO                           ENABLED
#   define VSF_USBH_USE_XB360                           ENABLED
#   define VSF_USBH_USE_DL1X5                           ENABLED
#   define VSF_USBH_USE_UAC                             ENABLED

#define VSF_USE_UI                                      ENABLED
#define VSF_USE_AUDIO                                   ENABLED
#   define VSF_AUDIO_USE_PLAYBACK                       ENABLED

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_USE_POSIX                                ENABLED
#   define VSF_LINUX_USE_SIMPLE_LIBC                    ENABLED
#   define VSF_LINUX_USE_BUSYBOX                        DISABLED
#   define VSF_LINUX_USE_SOCKET                         ENABLED
#       define VSF_LINUX_SOCKET_USE_UNIX                ENABLED
#       define VSF_LINUX_SOCKET_USE_INET                VSF_USE_TCPIP
#   define VSF_LINUX_USE_DEVFS                          ENABLED
#       define VSF_LINUX_DEVFS_USE_RAND                 ENABLED
#   define VSF_LINUX_CFG_FD_BITMAP_SIZE                 256
#   define VSF_LINUX_USE_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_LINUX_CFG_PLS_NUM                        16
#   define VSF_LINUX_CFG_TLS_NUM                        64
#   if VSF_USE_LOADER == ENABLED
#       define VSF_USE_APPLET                           ENABLED
#       define VSF_LINUX_USE_APPLET                     ENABLED
#   endif
#   define VSF_LINUX_CFG_MAX_ARG_NUM                    128
#   define VSF_LINUX_USE_SCRIPT                         ENABLED

// APP configuration
#define APP_USE_LINUX_DEMO                              ENABLED
#define APP_USE_LINUX_MOUNT_DEMO                        ENABLED
#define APP_USE_LINUX_DYNLOADER_DEMO                    ENABLED
#if VSF_USE_USB_HOST == ENABLED && VSF_LINUX_USE_LIBUSB == ENABLED
#   define APP_USE_LINUX_LIBUSB_DEMO                    ENABLED
#endif
#if VSF_LINUX_USE_SOCKET == ENABLED && VSF_LINUX_SOCKET_USE_INET == ENABLED
#   define APP_USE_LINUX_NTPDATE_DEMO                   ENABLED
#   define APP_USE_LINUX_TELNETD_DEMO                   ENABLED
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#endif
/* EOF */
