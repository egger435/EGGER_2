#include <stm32f10x.h>

#define USART1_TX_PIN GPIO_Pin_9  // USART1_TX
#define USART1_RX_PIN GPIO_Pin_10 // USART1_RX
#define USART1_BAUDRATE 115200 // 波特率115200

char Serial_RxString[100];  // 串口接收字符串

uint8_t Serial_RxFlag;  // 串口接收标志

// 串口通信初始化
void Serial_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // 使能USART1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 使能GPIOA时钟

    // 配置Tx引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN;           
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置Rx引脚
    GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART1参数
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = USART1_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能接收中断

    // 配置NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断优先级分组
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); // 初始化NVIC

    USART_Cmd(USART1, ENABLE);
}

// 发送一个字节
void Serial_SendByte(uint8_t byte)
{
    USART_SendData(USART1, byte);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); // 等待发送完成
}

// 发送字符串
void Serial_SendString(char *str)
{
    uint8_t i = 0;
    while (str[i] != '\0')
    {
        Serial_SendByte(str[i++]);
    }
}

// 获取接收标志位
uint8_t Serial_GetRxFlag(void)
{
    if (Serial_RxFlag == 1)
    {
        Serial_RxFlag = 0;
        return 1;
    }
    return 0;
}

// 串口中断函数,接收数据
// 字符数据包以@开始 *结束
void USART1_IRQHandler(void)
{
    static uint8_t RxState = 0;
    static uint8_t pRxString = 0;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        uint8_t RxData = USART_ReceiveData(USART1);

        if (RxState == 0)
        {
            if (RxData == '@')
            {
                RxState = 1;  // 接收数据开始
                pRxString = 0;
            }
        }
        else if (RxState == 1)
        {
            if (RxData == '*')  // 接收数据结束
            {
                RxState = 0;
                Serial_RxFlag = 1;
                Serial_RxString[pRxString] = '\0';  // 添加字符串结束符
            }
            else  // 持续接收数据
            {
                Serial_RxString[pRxString++] = RxData;
            }
        }

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}