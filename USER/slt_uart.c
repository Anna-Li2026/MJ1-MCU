#include "slt_uart.h"

/**
  * @brief  USART2 rx ISR. Line Terminator : DOS Terminators - CR/LF
  * @param
  * @retval
  */
static uint8_t slt_buf[SLT_UART_BUF_MAX_LEN] = {0};
static uint32_t slt_len = 0;
static uint8_t IDLE_STATUS = 0;
static void usart2_nvic_init(void);
static void usart2_hw_init(void);

void bsp_usart2_init(void)
{
    usart2_nvic_init();
    usart2_hw_init();
}

/**
 * @brief RXIE enable
 * @param
 * @param
 * @retval
 */
static void usart2_nvic_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable USARTy interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
 * @brief  USART2 gpio bound 8-N-1
 * @param  bound 92600bps, 2 bit stop, no parity, no hardware flow control
 * @param  tx_dma_buf, the dma buffer for usart2 tx
 * @retval dma_length, the dma length for usart2 tx
 */
static void usart2_hw_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* config USART2, GPIOA, DMA1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2 Rx (PA.3) as pull-up input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART2 mode config */
    USART_InitStructure.USART_BaudRate = SLT_UART_BOUND;
    USART_InitStructure.USART_WordLength = SLT_UART_DATA_BITS;
    USART_InitStructure.USART_StopBits = SLT_UART_STOP_BITS;
    USART_InitStructure.USART_Parity = SLT_UART_PARITY_BITS;
    USART_InitStructure.USART_HardwareFlowControl = SLT_UART_FLOW_CTRL;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     //TX & RX enable
    USART_Init(USART2, &USART_InitStructure);

    USART_ClearFlag(USART2, USART_FLAG_TC);                                             //clear TC 
    USART_ITConfig(USART2, USART_IT_ERR, ENABLE);                                       //Error interrupt(Frame error, noise error, overrun error)

//
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

    USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_ORE) != RESET)
    {
        USART_ReceiveData(USART2); //clear overrun pending
		USART_ClearITPendingBit(USART2, USART_IT_ORE);
    }
    if (USART_GetITStatus(USART2, USART_IT_FE) != RESET)
    {
        USART_ReceiveData(USART2); //clear fe pending
		USART_ClearITPendingBit(USART2, USART_IT_FE);
    }
    if (USART_GetITStatus(USART2, USART_IT_NE) != RESET)
    {
        USART_ReceiveData(USART2); //clear ne pending    
		USART_ClearITPendingBit(USART2, USART_IT_NE);
    }
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
		slt_buf[slt_len++] = USART_ReceiveData(USART2);
		if(slt_len == SLT_UART_BUF_MAX_LEN)slt_len = 0;
	}
	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) {
        volatile uint32_t temp_sr = USART2->SR;
        volatile uint32_t temp_dr = USART2->DR;
        (void)temp_sr;
        (void)temp_dr;
		IDLE_STATUS = 1;
    }
}
uint32_t get_usart2_buf(uint8_t* dest)
{
    uint32_t len = 0;
    
    if (dest == NULL) {
        return 0;  
    }
    
    __disable_irq();
    
    if (IDLE_STATUS && slt_len > 0) {
        len = (slt_len < SLT_UART_BUF_MAX_LEN) ? slt_len : SLT_UART_BUF_MAX_LEN;
        
        for (uint32_t i = 0; i < len; i++) {
            dest[i] = slt_buf[i];
        }
        
        slt_len = 0;
        IDLE_STATUS = 0;
    }
    
    __enable_irq();
    
    return len;
}
uint32_t get_usart2_buf_len()
{
	return slt_len;
}

uint8_t get_usart2_idle_status()
{
	return IDLE_STATUS;
}

void reset_usart2_idle_status()
{
	IDLE_STATUS = 0;
}
