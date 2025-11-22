/**
 ********************************************************************************
 * @file    ${dma.h}
 * @author  ${Quang Vinh Dang}
 * @date    ${November 22th, 2025}
 * @brief   
 * Doing dma stuff
 ********************************************************************************
 */
#ifndef __DMA_H__
#define __DMA_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/

/************************************
 * MACROS AND DEFINES
 ************************************/
#define DMA_RX_BUF_LEN 256

/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/
extern volatile char dma_rx_buf[DMA_RX_BUF_LEN];

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/

void dma_uart_rx_init(void); 


#ifdef __cplusplus
}
#endif

#endif //__DMA_H__
