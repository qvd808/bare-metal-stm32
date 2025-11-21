/**
 ********************************************************************************
 * @file    ${uart.h}
 * @author  ${Quang Vinh Dang}
 * @date    ${November 20th, 2025}
 * @brief   
 * Doing uart stuff
 ********************************************************************************
 */
#ifndef __UART_H__
#define __UART_H__

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

/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/

void uart_init(void); 
void uart_write_char(char c); 
void uart_write(const char *buf, uint32_t len); 


#ifdef __cplusplus
}
#endif

#endif //__UART_H__
