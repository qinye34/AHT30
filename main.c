#include"REG52.h"
#include"PLATFORM.h"
#include"UART.h"
#include"AHT20.h"
unsigned char adress[4]={0x70,0xAC,0x33,0x00};
unsigned char re_adress[1]={0x71};
unsigned char dat[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned long last_time = 0 ;
void main(void)
{

     UART_Init();
     Timer0_SysTick_Init(); // 定时器0 模式1
     APPLICATION_Aht20_Standard_IIC_Read_Address(adress,re_adress,dat);
    while(1)
    {
        APPLICATION_Aht20_Standard_IIC_Read_Task() ;
        if(Platform_Aht20_Standard_IIC_Read_Complete_Flag())
        {
        UART_Send_Str(dat, 7);
        Platform_Aht20_Standard_IIC_Read_Complete_Flag_Clear();
        }
        if((time_get_us() - last_time) >= 100000) 
        {
            APPLICATION_Aht20_Standard_IIC_Read_Address(adress,re_adress,dat);
            last_time = time_get_us();
        }
        
    }
  
}