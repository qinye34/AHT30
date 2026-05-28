#include "UART.h"

static unsigned char Uart_Rx_Buffer[Uart_Rx_Buffer_Len];
static unsigned char Uart_Rx_Count= 0 ;
static bit Uart_Rx_Flag ;


static void UART_Send_Str_Change(unsigned char *str, unsigned char len)
{
    Platform_UART_Send_Str(str, len);
}

static void UART_Init_Change(void)
{
    Platform_UART_Init();  // 只调用平台
}

UART_TypeDef Uart ={UART_Init_Change,UART_Send_Str_Change} ;

void UART_Init()
{
Uart.Init();
}


void UART_Send_Str(unsigned char *str, unsigned char len)
{
Uart.Send_Str(str, len);
}

void UART_Isr(void) interrupt 4
{
    if(RI)
    {
        RI = 0;
		 if(Uart_Rx_Count < Uart_Rx_Buffer_Len-1)
        {
            Uart_Rx_Buffer[Uart_Rx_Count++] = SBUF;
        }
        Uart_Rx_Flag = 1 ;
    }
	
}


unsigned char* UART_Receive()
{
    if(Uart_Rx_Flag)
    {
	    Uart_Rx_Buffer[Uart_Rx_Count] = '\0'; // '\0'=0x00 
        return Uart_Rx_Buffer;
    }
    return 0;
}

void UART_Clear(void)
{
	unsigned char i ;
    Uart_Rx_Flag = 0;
    Uart_Rx_Count = 0;
	  for(i=0; i<Uart_Rx_Buffer_Len; i++)
    {
        Uart_Rx_Buffer[i] = 0;
    }
}