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

/*============================ INCLUDES ======================================*/

#define __VSF_HEAP_CLASS_INHERIT__
#include "./vsf_board.h"

#include "board.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/******************************************************************************/
/*  DEBUG_STREAM
/******************************************************************************/

#include "hpm_uart_drv.h"
#define VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE         32
static uint8_t __vsf_debug_stream_rx_buff[VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE];
vsf_mem_stream_t VSF_DEBUG_STREAM_RX = {
    .op         = &vsf_mem_stream_op,
    .buffer     = __vsf_debug_stream_rx_buff,
    .size       = sizeof(__vsf_debug_stream_rx_buff),
};
static void __uart_isr(void)
{
    uint8_t buff[uart_get_fifo_size(BOARD_APP_UART_BASE)], buff_index = 0;

    while (uart_get_irq_id(BOARD_APP_UART_BASE) & uart_intr_id_rx_data_avail) {
        if (status_success != uart_receive_byte(BOARD_APP_UART_BASE, &buff[buff_index++])) {
            VSF_ASSERT(false);
        }
    }
    if (buff_index) {
        VSF_STREAM_WRITE(&VSF_DEBUG_STREAM_RX, buff, buff_index);
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_UART_IRQ, __uart_isr)
static void __VSF_DEBUG_STREAM_TX_INIT(void)
{
    vsf_stream_connect_tx(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t);
    uart_enable_irq(BOARD_APP_UART_BASE, uart_intr_rx_data_avail_or_timeout);
    intc_m_enable_irq_with_priority(BOARD_APP_UART_IRQ, 1);
}
static void __VSF_DEBUG_STREAM_TX_WRITE_BLOCKED(uint8_t *buf, uint_fast32_t size)
{
    for (uint_fast32_t i = 0; i < size; i++) {
        uart_send_byte(BOARD_APP_UART_BASE, *buf++);
    }
}

#define VSF_HAL_USE_DEBUG_STREAM        ENABLED
#include "hal/driver/common/debug_stream/debug_stream_tx_blocked.inc"

void vsf_arch_shutdown(void)
{

}

void vsf_arch_reset(void)
{
    
}

void vsf_board_init(void)
{
    board_init();

    // do not change order below
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_RX);
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_TX);
}
