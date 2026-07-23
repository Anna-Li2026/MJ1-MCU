#ifndef CMD_PROC_H
#define  CMD_PROC_H

#include "stm32f10x.h"
#include "bsp_uart4.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "slt_uart.h"

//match the command from TCP server
#define CMD_PARAM_CONFIG   				"@SParam"
#define RESULT_PARAM_CONFIG_PASS   		"@SParamPass#E@"
#define CMD_POWER_ON     				"@SPowerOn#E@"
#define RESULT_POWER_ON_PASS     		"@SPowerOnPass#E@"
#define CMD_POWER_DOWN	     			"@SPowerDown#E@"
#define RESULT_POWER_DOWN_PASS	     	"@SPowerDownPass#E@"

#define PC_USART						USART1
#define SLT_USART						USART2


typedef struct {
    uint16_t gpio_status;
	uint32_t checksum;      		// checksum
}ParamConfigParams_t;

int pc_process_received_command(uint8_t* cmd, uint32_t len);
int slt_process_received_command(uint8_t* cmd, uint32_t len);

#endif
