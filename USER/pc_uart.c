#include "pc_uart.h"

/**
  * @brief  USART1 rx ISR. Line Terminator : DOS Terminators - CR/LF
  * @param
  * @retval
  */
static uint8_t pc_buf[100] = {0};
static uint8_t pc_len = 0;
static uint8_t IDLE_STATUS = 0;
static void usart1_nvic_init(void);
static void usart1_hw_init(void);

void bsp_usart1_init(void)
{
    usart1_nvic_init();
    usart1_hw_init();
}

/**
 * @brief RXIE enable
 * @param
 * @param
 * @retval
 */
static void usart1_nvic_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable USARTy interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
 * @brief  usart1 gpio bound 8-N-1
 * @param  bound 92600bps, 2 bit stop, no parity, no hardware flow control
 * @param  tx_dma_buf, the dma buffer for usart2 tx
 * @retval dma_length, the dma length for usart2 tx
 */
static void usart1_hw_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* config USART1, GPIOA, DMA1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Rx (PA.10) as pull-up input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = PC_UART_BOUND;
    USART_InitStructure.USART_WordLength = PC_UART_DATA_BITS;
    USART_InitStructure.USART_StopBits = PC_UART_STOP_BITS;
    USART_InitStructure.USART_Parity = PC_UART_PARITY_BITS;
    USART_InitStructure.USART_HardwareFlowControl = PC_UART_FLOW_CTRL;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     //TX & RX enable
    USART_Init(USART1, &USART_InitStructure);

    USART_ClearFlag(USART1, USART_FLAG_TC);                                             //clear TC 
    USART_ITConfig(USART1, USART_IT_ERR, ENABLE);                                       //Error interrupt(Frame error, noise error, overrun error)

//
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

    USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_ORE) != RESET)
    {
        USART_ReceiveData(USART1); //clear overrun pending
		USART_ClearITPendingBit(USART1, USART_IT_ORE);
    }
    if (USART_GetITStatus(USART1, USART_IT_FE) != RESET)
    {
        USART_ReceiveData(USART1); //clear fe pending
		USART_ClearITPendingBit(USART1, USART_IT_FE);
    }
    if (USART_GetITStatus(USART1, USART_IT_NE) != RESET)
    {
        USART_ReceiveData(USART1); //clear ne pending    
		USART_ClearITPendingBit(USART1, USART_IT_NE);
    }
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
		pc_buf[pc_len++] = USART_ReceiveData(USART1);
		if(pc_len == 100)pc_len = 0;
	}
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
        volatile uint32_t temp_sr = USART1->SR;
        volatile uint32_t temp_dr = USART1->DR;
        (void)temp_sr;
        (void)temp_dr;
		IDLE_STATUS = 1;
    }
}
uint8_t get_usart1_buf(uint8_t* dest)
{
    uint8_t len = 0;
    
    if (dest == NULL) {
        return 0;  // ????
    }
    
    __disable_irq();
    
    if (IDLE_STATUS && pc_len > 0) {
        len = (pc_len < 100) ? pc_len : 100;
        
        for (uint8_t i = 0; i < len; i++) {
            dest[i] = pc_buf[i];
        }
        
        pc_len = 0;
        IDLE_STATUS = 0;
    }
    
    __enable_irq();
    
    return len;
}
uint8_t get_usart1_buf_len()
{
	return pc_len;
}

uint8_t get_usart1_idle_status()
{
	return IDLE_STATUS;
}

void reset_usart1_idle_status()
{
	IDLE_STATUS = 0;
}
