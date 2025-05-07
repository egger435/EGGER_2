/*
 * ************************************************
 * EGGER DYNAMICS
 * EGGER 2
 * ************************************************
*/
#include "stm32f10x.h"
#include ".\Modules\Serial.h"
#include ".\Modules\PWM.h"
#include ".\Modules\Motor.h"

void Setup(void)
{
    Serial_Init();
    Serial_SendString("Start");

    Motor_Init();
    Motor_Set_Speed(0);
}

void Update(void)
{
    if (Serial_GetRxFlag() == 1)
    {
        Motor_CMD(Serial_RxString);
    }
}

int main()
{
    Setup();
    while(1)
    {
        Update();
    }
    return 0;
}
