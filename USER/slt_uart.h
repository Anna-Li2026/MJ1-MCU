#ifndef SLT_UART_H
#define  SLT_UART_H

#include "stm32f10x.h"
#include "string.h"

#define SLT_UART_BOUND                    (921600)
#define SLT_UART_DATA_BITS                USART_WordLength_8b
#define SLT_UART_STOP_BITS                USART_StopBits_1
#define SLT_UART_PARITY_BITS              USART_Parity_No
#define SLT_UART_FLOW_CTRL                USART_HardwareFlowControl_None

#define SLT_UART_BUF_MAX_LEN		3000U


uint8_t get_usart2_idle_status(void);
uint32_t get_usart2_buf_len(void);
void reset_usart2_idle_status(void);
uint32_t get_usart2_buf(uint8_t* dest);
void bsp_usart2_init(void);


#endif
