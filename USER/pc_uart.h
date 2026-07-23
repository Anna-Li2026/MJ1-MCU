#ifndef PC_UART_H
#define  PC_UART_H

#include "stm32f10x.h"
#include "string.h"

#define PC_UART_BOUND                    (115200)
#define PC_UART_DATA_BITS                USART_WordLength_8b
#define PC_UART_STOP_BITS                USART_StopBits_1
#define PC_UART_PARITY_BITS              USART_Parity_No
#define PC_UART_FLOW_CTRL                USART_HardwareFlowControl_None

uint8_t get_usart1_idle_status(void);
uint8_t get_usart1_buf_len(void);
void reset_usart1_idle_status(void);
uint8_t get_usart1_buf(uint8_t* dest);
void bsp_usart1_init(void);


#endif
