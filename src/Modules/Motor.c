#include <stm32f10x.h>
#include <.\Modules\PWM.h>
#include <.\Modules\Serial.h>

// A路电机控制前驱动轮
// 1号电机右轮，2号电机左轮
#define MOTOR_A1_PIN1 GPIO_Pin_4  // A路1号电机引脚1  PA4
#define MOTOR_A1_PIN2 GPIO_Pin_5  // A路1号电机引脚2  PA5
#define MOTOR_A2_PIN1 GPIO_Pin_6  // A路2号电机引脚1  PA6
#define MOTOR_A2_PIN2 GPIO_Pin_7  // A路2号电机引脚2  PA7

// B路电机控制后驱动轮
// 1号电机右轮，2号电机左轮
#define MOTOR_B1_PIN1 GPIO_Pin_0  // B路1号电机引脚1  PB0
#define MOTOR_B1_PIN2 GPIO_Pin_1  // B路1号电机引脚2  PB1
#define MOTOR_B2_PIN1 GPIO_Pin_10 // B路2号电机引脚1  PB10
#define MOTOR_B2_PIN2 GPIO_Pin_11 // B路2号电机引脚2  PB11

void Motor_Init(void)
{
    PWM_Init();
    PWM_SetDutyCycle(0);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MOTOR_A1_PIN1 | MOTOR_A1_PIN2 | MOTOR_A2_PIN1 | MOTOR_A2_PIN2; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化A路电机引脚

    GPIO_InitStructure.GPIO_Pin = MOTOR_B1_PIN1 | MOTOR_B1_PIN2 | MOTOR_B2_PIN1 | MOTOR_B2_PIN2; 
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 初始化B路电机引脚

    // 设置初始状态为停止
    GPIO_SetBits(GPIOA, MOTOR_A1_PIN1 | MOTOR_A1_PIN2 | MOTOR_A2_PIN1 | MOTOR_A2_PIN2); 
    GPIO_SetBits(GPIOB, MOTOR_B1_PIN1 | MOTOR_B1_PIN2 | MOTOR_B2_PIN1 | MOTOR_B2_PIN2); 
}

// 设置电机速度
void Motor_Set_Speed(int8_t speed)
{
    // 1号引脚高，2号引脚低正转
    if (speed > 0)
    {
        GPIO_SetBits(GPIOA, MOTOR_A1_PIN1 | MOTOR_A2_PIN1);
        GPIO_SetBits(GPIOB, MOTOR_B1_PIN1 | MOTOR_B2_PIN1);
        GPIO_ResetBits(GPIOA, MOTOR_A1_PIN2 | MOTOR_A2_PIN2);
        GPIO_ResetBits(GPIOB, MOTOR_B1_PIN2 | MOTOR_B2_PIN2);
        PWM_SetDutyCycle(speed);
    }
    else // 2号引脚高，1号引脚低正转
    {
        GPIO_SetBits(GPIOA, MOTOR_A1_PIN2 | MOTOR_A2_PIN2);
        GPIO_SetBits(GPIOB, MOTOR_B1_PIN2 | MOTOR_B2_PIN2);
        GPIO_ResetBits(GPIOA, MOTOR_A1_PIN1 | MOTOR_A2_PIN1);
        GPIO_ResetBits(GPIOB, MOTOR_B1_PIN1 | MOTOR_B2_PIN1);
        PWM_SetDutyCycle(-speed);
    }
}

// 电机立刻制动
void Motor_Stop(void)
{
    GPIO_SetBits(GPIOA, MOTOR_A1_PIN1 | MOTOR_A1_PIN2 | MOTOR_A2_PIN1 | MOTOR_A2_PIN2); 
    GPIO_SetBits(GPIOB, MOTOR_B1_PIN1 | MOTOR_B1_PIN2 | MOTOR_B2_PIN1 | MOTOR_B2_PIN2); 
    PWM_SetDutyCycle(0);
}

// 电机控制命令处理函数
// 命令：f_x, b_x, s, l, r
void Motor_CMD(char *cmd)
{
    if (cmd[0] == 's')  // 立即制动
    {
        Motor_Stop();
        Serial_SendString("Motor Stop");
    }
    else if (cmd[0] == 'f')  // 前进
    {
        switch (cmd[2])
        {
        case '1':
            Motor_Set_Speed(30);
            Serial_SendString("Motor Forward 30");
            break;
        case '2':
            Motor_Set_Speed(50);
            Serial_SendString("Motor Forward 50");
            break;
        case '3':
            Motor_Set_Speed(100);
            Serial_SendString("Motor Forward 100");
            break;
        default:
            break;
        }
    }
    else if (cmd[0] == 'b')  // 后退
    {
        switch (cmd[2])
        {
        case '1':
            Motor_Set_Speed(-30);
            Serial_SendString("Motor Backward 30");
            break;
        case '2':
            Motor_Set_Speed(-50);
            Serial_SendString("Motor Backward 50");
            break;
        case '3':
            Motor_Set_Speed(-100);
            Serial_SendString("Motor Backward 100");
            break;
        default:
            break;
        }
    }
    else if (cmd[0] == 'l')  // 左转，右轮正转，左轮反转
    {
        Motor_Stop();
        // 右轮正转，两路的1号电机的引脚1高，引脚2低
        GPIO_SetBits(GPIOA, MOTOR_A1_PIN1);
        GPIO_SetBits(GPIOB, MOTOR_B1_PIN1);
        GPIO_ResetBits(GPIOA, MOTOR_A1_PIN2);
        GPIO_ResetBits(GPIOB, MOTOR_B1_PIN2);
        // 左轮反转，两路的2号电机的引脚1低，引脚2高
        GPIO_SetBits(GPIOA, MOTOR_A2_PIN2);
        GPIO_SetBits(GPIOB, MOTOR_B2_PIN2);
        GPIO_ResetBits(GPIOA, MOTOR_A2_PIN1);
        GPIO_ResetBits(GPIOB, MOTOR_B2_PIN1);
        // 设置PWM占空比为50%
        PWM_SetDutyCycle(50);
        Serial_SendString("Motor Left Turn 50");
        
    }
    else if (cmd[0] == 'r')  // 右转，左轮正传，右轮反转
    {
        Motor_Stop();
        // 左轮正转，两路的2号电机的引脚1高，引脚2低
        GPIO_SetBits(GPIOA, MOTOR_A2_PIN1);
        GPIO_SetBits(GPIOB, MOTOR_B2_PIN1);
        GPIO_ResetBits(GPIOA, MOTOR_A2_PIN2);
        GPIO_ResetBits(GPIOB, MOTOR_B2_PIN2);
        // 右轮反转，两路的1号电机的引脚1低，引脚2高
        GPIO_SetBits(GPIOA, MOTOR_A1_PIN2);
        GPIO_SetBits(GPIOB, MOTOR_B1_PIN2);
        GPIO_ResetBits(GPIOA, MOTOR_A1_PIN1);
        GPIO_ResetBits(GPIOB, MOTOR_B1_PIN1);
        // 设置PWM占空比为50%
        PWM_SetDutyCycle(50);
        Serial_SendString("Motor Right Turn 50");
    }
    else
    {
        Serial_SendString("Invalid CMD");
    }
}