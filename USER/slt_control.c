#include "slt_control.h"

static const GpioMap_t gpio_map[10] = {
    {IO9_PORT, IO9_PIN},  // gpio[9] -> PB3
    {IO8_PORT, IO8_PIN},  // gpio[8] -> PC3
    {IO7_PORT, IO7_PIN},  // gpio[7] -> PB4
    {IO6_PORT, IO6_PIN},  // gpio[6] -> PC2
    {IO5_PORT, IO5_PIN},  // gpio[5] -> PB5
    {IO4_PORT, IO4_PIN},  // gpio[4] -> PC1
    {IO3_PORT, IO3_PIN},  // gpio[3] -> PB6
    {IO2_PORT, IO2_PIN},  // gpio[2] -> PC0
    {IO1_PORT, IO1_PIN},  // gpio[1] -> PB7
    {IO0_PORT, IO0_PIN}   // gpio[0] -> PC6
};

void GPIO_Output_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t InitLevel)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    if (GPIOx == GPIOA) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    } else if (GPIOx == GPIOB) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    } else if (GPIOx == GPIOC) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    } else if (GPIOx == GPIOD) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    } else if (GPIOx == GPIOE) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    }
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    
    if (InitLevel == 0) {
        GPIO_ResetBits(GPIOx, GPIO_Pin);   
    } else {
        GPIO_SetBits(GPIOx, GPIO_Pin);     
    }
//	uint8_t level = GPIO_ReadOutputDataBit(GPIOx, GPIO_Pin);
//	printf("Pin level after init: %d\n", level);
}

void SLT_GPIO_Init()
{
	GPIO_Output_Init(IO0_PORT, IO0_PIN, 0);
	GPIO_Output_Init(IO1_PORT, IO1_PIN, 0);
	GPIO_Output_Init(IO2_PORT, IO2_PIN, 0);
	GPIO_Output_Init(IO3_PORT, IO3_PIN, 0);
	GPIO_Output_Init(IO4_PORT, IO4_PIN, 0);
	GPIO_Output_Init(IO5_PORT, IO5_PIN, 0);
	GPIO_Output_Init(IO6_PORT, IO6_PIN, 0);
	GPIO_Output_Init(IO7_PORT, IO7_PIN, 0);
	GPIO_Output_Init(IO8_PORT, IO8_PIN, 0);
	GPIO_Output_Init(IO9_PORT, IO9_PIN, 0);
	GPIO_Output_Init(VCC_PORT, VCC_PIN, 0);
}

void Set_SLT_GPIO(uint8_t i)
{
	GPIO_SetBits(gpio_map[i].port, gpio_map[i].pin);
}

void Reset_SLT_GPIO(uint8_t i)
{
	GPIO_ResetBits(gpio_map[i].port, gpio_map[i].pin);
}
