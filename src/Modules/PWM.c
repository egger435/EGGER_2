#include <stm32f10x.h>

#define PWM1_OUTPUT_PIN GPIO_Pin_2 // PWM1输出引脚 TIM2_CH3
#define PWM2_OUTPUT_PIN GPIO_Pin_3 // PWM2输出引脚 TIM2_CH4
#define PWM3_OUTPUT_PIN GPIO_Pin_0 // PWM3输出引脚 TIM2_CH1
#define PWM4_OUTPUT_PIN GPIO_Pin_1 // PWM4输出引脚 TIM2_CH2

void PWM_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // 使能TIM2时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟

    // 设置PWM输出引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = PWM1_OUTPUT_PIN | PWM2_OUTPUT_PIN | PWM3_OUTPUT_PIN | PWM4_OUTPUT_PIN; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    // 配置TIM2
    TIM_InternalClockConfig(TIM2);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 36 - 1; 
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 配置PWM模式
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure); // 初始化结构体
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
    TIM_OCInitStructure.TIM_Pulse = 0; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // 配置TIM2四个通道
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC3Init(TIM2, &TIM_OCInitStructure); 
    TIM_OC4Init(TIM2, &TIM_OCInitStructure); 

    TIM_Cmd(TIM2, ENABLE); // 使能TIM2
}

// 设置PWM占空比
// 通道1到4的占空比相同
void PWM_SetDutyCycle(uint16_t duty)
{
    if (duty > 100) duty = 100; // 限制占空比范围
    if (duty == 100)
    {
        TIM_SetCompare1(TIM2, TIM2->ARR + 1);
        TIM_SetCompare2(TIM2, TIM2->ARR + 1);
        TIM_SetCompare3(TIM2, TIM2->ARR + 1);
        TIM_SetCompare4(TIM2, TIM2->ARR + 1);
    }
    else
    {
        TIM_SetCompare1(TIM2, duty);
        TIM_SetCompare2(TIM2, duty);
        TIM_SetCompare3(TIM2, duty); // 设置TIM2_CH3的占空比
        TIM_SetCompare4(TIM2, duty); // 设置TIM2_CH4的占空比
    }
}