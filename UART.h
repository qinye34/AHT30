#ifndef __UART_H__
#define __UART_H__

#include "platform.h"

#define Uart_Rx_Buffer_Len 4

typedef struct
{
  void(*Init)(void) ;
  void(*Send_Str)(unsigned char *str, unsigned char len) ;
}UART_TypeDef ;


void UART_Init(void);
void UART_Send_Str(unsigned char *str, unsigned char len);
unsigned char* UART_Receive(void);
void UART_Clear(void);

#endif

