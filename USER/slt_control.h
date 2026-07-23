#ifndef SLT_CONTRL_H
#define  SLT_CONTRL_H

#include "stm32f10x.h"

#define IO0_PORT	GPIOC
#define IO1_PORT	GPIOB
#define IO2_PORT	GPIOC
#define IO3_PORT	GPIOB
#define IO4_PORT	GPIOC
#define IO5_PORT	GPIOB
#define IO6_PORT	GPIOC
#define IO7_PORT	GPIOB
#define IO8_PORT	GPIOC
#define IO9_PORT	GPIOB

#define IO0_PIN		GPIO_Pin_6
#define IO1_PIN		GPIO_Pin_7
#define IO2_PIN		GPIO_Pin_0
#define IO3_PIN		GPIO_Pin_6
#define IO4_PIN		GPIO_Pin_1
#define IO5_PIN		GPIO_Pin_5
#define IO6_PIN		GPIO_Pin_2
#define IO7_PIN		GPIO_Pin_4
#define IO8_PIN		GPIO_Pin_3
#define IO9_PIN		GPIO_Pin_3

#define VCC_PORT	GPIOB
#define VCC_PIN		GPIO_Pin_14

 typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} GpioMap_t;
    
void GPIO_Output_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t InitLevel);
void SLT_GPIO_Init(void);
void Set_SLT_GPIO(uint8_t i);
void Reset_SLT_GPIO(uint8_t i);

#endif
