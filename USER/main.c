
/**
* @file main.c
*
* Copyright (c) Artmem 2022. All Rights Reserved.
*
* @brief main
*
* @details W5500 works in server mode, waiting for the pc client to actively connect.
*           After the connection is successful, processing the transaction
*
*           STM32 - W5500
*           PC5 - W5500_RST
*           PC4 - W5500_INT
*           PA4 - W5500_SCS
*           PA5 - W5500_SCK
*           PA6 - W5500_MISO
*           PA7 - W5500_MOSI
*
* @date 2022/9/2
*
* @author heyy
*
* @bug
*
* Revisions: v1.0
*
*/
#include "../version"
#include <string.h>
#include <stdint.h>
#include "stm32f10x.h"
#include "SysTick.h"
//#include "trace.h"
//#include "bsp_timer2.h"
#include "bsp_uart4.h"
//#include "bsp_inter_xfer.h"
//#include "bsp_usart3.h"
//#include "bsp_eeprom.h"
//#include "nr_micro_shell.h"
//#include "socket.h"             //just include one header for WIZChip
//#include "dhcp.h"
#include "protocol.h"
//#include "bsp_burnin_board.h"
//#include "burnin_board_map.h"
#include "pc_uart.h"
#include "slt_uart.h"
#include "cmd_proc.h"
#include "slt_control.h"


////////////////////////////////////////////////
// Shared Buffer Definition for project       //
////////////////////////////////////////////////
//head size sizeof(ITVL_t)
//content size 32KB
//end '\r\n\0'
//uint8_t gDATABUF[8192 - MAX_TRANSMISSION_LENGTH] __attribute__((aligned(8)));
//uint8_t  remote_ip[4]={192,168,1,2};											
//uint16_t remote_port=5000;	
//ITVL_t* gITVL = (ITVL_t*)gDATABUF;
//ITVL_t itvl;
//ITVL_t itvl_recv;
//ITVL_t itvl_tim;
//uint8_t itvl_init_flag=0; 
//volatile uint32_t sec = 0;

/////////////////////////////////////////
// SOCKET NUMBER DEFINION for Examples //
/////////////////////////////////////////
//#define SOCK_TCPS        0
//#define SOCK_UDPS        6
//#define SOCK_TCPC        0

///******IP******/
//uint8_t local_ip[4]  ={192,168,1,88};											
//uint8_t subnet[4]    ={255,255,255,0};										
//uint8_t gateway[4]   ={192,168,1,1};										
//uint8_t dns_server[4]={114,114,114,114};									
//uint16_t local_port=5000;	 

//uint8_t xh=0;

#if 0
#define BUFF_SIZE   2048
uint8_t downlink[BUFF_SIZE] __attribute__((aligned(8)));
uint8_t uplink[BUFF_SIZE]   __attribute__((aligned(8)));
#endif

#if 0
/**
 * @brief stm32 system PLL 72MHZ
 *
 */
static void RCC_Configuration(void)
{
    ErrorStatus HSEStartUpStatus;                              //HSE work status

    RCC_DeInit();                                              //all registers about clock set default
    RCC_HSEConfig(RCC_HSE_ON);                                 //HSE switch on
    HSEStartUpStatus = RCC_WaitForHSEStartUp();                //wait for HSE to stabilize

    if (SUCCESS == HSEStartUpStatus)                            //stabilized
    {
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);    //set FLASH
        /* Flash 2 wait state */
        FLASH_SetLatency(FLASH_Latency_2);


        RCC_HCLKConfig(RCC_SYSCLK_Div1);                         //AHB 72/1MHZ
        RCC_PCLK2Config(RCC_HCLK_Div1);                          //APB2 and HCLK 72MHZ
        RCC_PCLK1Config(RCC_HCLK_Div2);                          //APB1 72/2MHz

#ifndef STM32F10X_CL
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);     //PLLCLK = 8MHz * 9 = 72 MHz 
#else
        /* MCU are STM32F107x or STM32F105x */
        /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
        RCC_PREDIV2Config(RCC_PREDIV2_Div5);
        RCC_PLL2Config(RCC_PLL2Mul_8);

        RCC_PLL2Cmd(ENABLE);                                     //enable PLL2
        while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET);    //wait for stabilize

        /* PLL configuration: PLLCLK = (PLL2 / 5) * 9 = 72 MHz */
        RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
        RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#endif

        RCC_PLLCmd(ENABLE);                                         //enable PLL
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);        //wait for stabilize

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);                  //system clock source is PLL

        while (RCC_GetSYSCLKSource() != 0x08);                      //check clock source
        RCC_ClockSecuritySystemCmd(ENABLE);                         //enable clock security

        /* Enable peripheral clocks --------------------------------------------------*/
        /* Enable I2C1 and I2C1 clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

        /* Enable GPIOA GPIOB SPI1 and USART1 clocks */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
                               | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
                               | RCC_APB2Periph_AFIO, ENABLE);
    }
}
#endif

/**
 * @brief stm32 interrupt vector table configuration
 */
static void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//Set 2bit preempt priority and 2bit subpriority group

/*  preempt    subpriority    interrupt
 *      2        3            W5500 EXIT
 *      3        0            USART2
 *      3        1            USART3
 *      3        2            USART1
 *      3        3            TIMER2
 */

    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);    //Set the Vector Table base location at 0x08000000
}



/**
 * @brief platform (STM32F103X) initialization for peripherals as GPIO, SPI, UARTs
 */
static void System_Initialization(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    //RCC_Configuration();                                  //System clock 72MHZ
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    RCC_GetClocksFreq(&rcc_clocks);
    NVIC_Configuration();                                   //STM32 interrupt vector

    SysTick_Init(rcc_clocks.SYSCLK_Frequency/1000000);      //System Tick

	bsp_usart1_init();
	bsp_usart2_init();
	SLT_GPIO_Init();
}

static uint8_t pc_recv_data[100] = {0};
static uint8_t pc_recv_len = 0;

void pc_uart_proccess()
{
	pc_recv_len = get_usart1_buf(pc_recv_data);
	if(pc_recv_len != 0){
//		Usart_SendString(USART1,(char*)pc_recv_data);
		pc_process_received_command(pc_recv_data,pc_recv_len);
		memset(pc_recv_data,0,100);
	}
}

static uint8_t slt_recv_data[SLT_UART_BUF_MAX_LEN] = {0};
static uint32_t slt_recv_len = 0;
void slt_uart_proccess()
{
	slt_recv_len = get_usart2_buf(slt_recv_data);
	if(slt_recv_len != 0){
//		Usart_SendString(USART1,(char*)slt_recv_data);
		slt_process_received_command(slt_recv_data, slt_recv_len);
		memset(slt_recv_data,0,SLT_UART_BUF_MAX_LEN);
	}
}
/**
 * @brief main
 */

int main(void)
{
    System_Initialization();
//	Usart_SendString(USART1,"Init usart1 sucessful!\r\n");
//	Usart_SendString(USART2,"Init usart2 sucessful!\r\n");
	
    while (1)
    {
		pc_uart_proccess();
		slt_uart_proccess();
    }

}//end of main()
