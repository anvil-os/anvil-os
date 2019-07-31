
#include "debug.h"
#include "stm32f10x.h"

int debug_init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_9;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);

    gpio_init.GPIO_Pin = GPIO_Pin_10;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);

    USART_InitTypeDef usartinit;
    usartinit.USART_BaudRate = 115200;
    usartinit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usartinit.USART_Mode = USART_Mode_Tx;
    usartinit.USART_Parity = USART_Parity_No;
    usartinit.USART_StopBits = USART_StopBits_1;
    usartinit.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &usartinit);
    USART_Cmd(USART1, ENABLE);
    return 0;
}

void debug_putc(int ch)
{
    while (!(USART1->SR & USART_SR_TXE))
    {
    }
    USART1->DR = ch;
}
