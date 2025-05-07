// 电机控制模块
#ifndef __MOTOR_C
#define __MOTOR_C

void Motor_Init(void);
void Motor_Set_Speed(int8_t speed);
void Motor_CMD(char *cmd);

#endif