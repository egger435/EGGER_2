// 蓝牙串口通信模块
#ifndef __SERIAL_H
#define __SERIAL_H

extern char Serial_RxString[100];

void Serial_Init(void);
void Serial_SendByte(uint8_t byte);
void Serial_SendString(char *str);
uint8_t Serial_GetRxFlag(void);

#endif