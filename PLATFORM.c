#include "platform.h"

volatile unsigned long System_Tick = 0;
unsigned long now ;

//==================== AT24C0C 设备结构体封装 ====================//
typedef struct 
{
    unsigned long        IIC_Wait_Tick;
    unsigned char        IIC_Rx_Buffer;   
      unsigned char      *IIC_Rx_Adress;  
      unsigned char      *IIC_Rx_Re_Adress;   
      unsigned char       *IIC_Rx_Data  ;
    unsigned char        IIC_Rx_Buffer_Long; // 读长度
    unsigned char        IIC_Bit_Cnt;
    unsigned char        ack;
    unsigned char       IIC_Read_Complete ;
    Aht20_I2C_Read_State_Typedef   Read_State;
    Aht20_I2C_Read_Event_Typedef   Read_Event;
} AHT20_TypeDef;

static AHT20_TypeDef AHT20_READ = {
    0
};


//=================================================================

#define IIC_DELAY_5US 5
#define IIC_DELAY_3US 3

sbit IO_SCL = P1^0;
sbit IO_SDA = P1^1;
sbit IO_BEEP     = P1^5;


void GPIO_Set(Pin_ID_TypeDef pin, unsigned char state)
{
    switch (pin)
    {
        case PIN_SCL:
            IO_SCL = state;
            break;
        case PIN_SDA:
            IO_SDA = state;
            break;
        case PIN_BEEP:
            IO_BEEP = state;
            break;
        default:
            break;
    }
}

unsigned char GPIO_Get(Pin_ID_TypeDef pin)
{
    switch (pin)
    {
        case PIN_SDA:
            return IO_SDA;
         case PIN_BEEP:
             return IO_BEEP;
        case PIN_SCL:
            return IO_SCL;
        default:
            return 0;
    }
}

void Timer0_SysTick_Init(void)
{
    TMOD |= 0x01;   // Timer0 模式1 16位
    // 1us 重装初值
    TH0 = 0xFF;
    TL0 = 0xFF;

    ET0 = 1;    // 使能定时器0中断
    TR0 = 1;    // 启动定时器
    EA  = 1;    // 开总中断
}

void Timer0_ISR(void) interrupt 1
{
    // 重装1us初值
    TH0 = 0xFF;
    TL0 = 0xFF;
    System_Tick++;  // 全局时间+1
}

unsigned long time_get_us(void)
{
    return System_Tick;
}

// 12MHz 晶振
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 120; j++);
}





void Platform_UART_Init(void)
{
    SCON = 0x50;    // 51专属
    TMOD |= 0x20;
    TH1 = 0xFD;
    TL1 = 0xFD;
	
	TR1 = 1;
		
    ES = 1;
    EA = 1;
}

void Platform_UART_Send_Str(unsigned char *str, unsigned char len)
{
     while (len--) 
     {
        SBUF = *str++;
        while (!TI);
        TI = 0;
    }
}

void Platform_Aht20_Standard_IIC_Read_Address(unsigned char *adress,unsigned char *re_adress,unsigned char *dat)
{
    AHT20_READ.IIC_Rx_Adress = adress;
    AHT20_READ.IIC_Rx_Re_Adress = re_adress;
    AHT20_READ.IIC_Rx_Data = dat;
    AHT20_READ.Read_Event = READ_I2C_EVENT_START; // 进入起始条件阶段
    AHT20_READ.Read_State = READ_I2C_START_STEP1; // 起始条件的第一步
    AHT20_READ.IIC_Read_Complete = 0; 
    AHT20_READ.IIC_Bit_Cnt = 0;
    AHT20_READ.IIC_Rx_Buffer_Long = 0;
}




void Platform_Aht20_Standard_IIC_Read_Start(void)
{
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
        // 起始条件：SCL和SDA都为高电平
       case READ_I2C_START_STEP1 :
    GPIO_Set(PIN_SDA, 1);
    GPIO_Set(PIN_SCL, 1);
     AHT20_READ.IIC_Wait_Tick =now ;
     AHT20_READ.Read_State = READ_I2C_START_STEP2;
    break;
        // 起始条件：等待5us，SDA在SCL高电平期间由高变低
      case READ_I2C_START_STEP2 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        GPIO_Set(PIN_SDA, 0);
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_START_STEP3;
    }
    break;
   // 起始条件：直接把scl为1
      case READ_I2C_START_STEP3 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        GPIO_Set(PIN_SCL, 0);
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_Event = READ_I2C_EVENT_ADDRESS; // 进入发送地址阶段
        AHT20_READ.Read_State = READ_I2C_SEND_ADDRESS_STEP1;
    }
    break;
}
}

void Platform_Aht20_Standard_IIC_Read_Devidce_And_Word_Address(void)
{   
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
        //等待3us，改变sda
    case READ_I2C_SEND_ADDRESS_STEP1 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        AHT20_READ.IIC_Rx_Buffer = *(AHT20_READ.IIC_Rx_Adress); 
        AHT20_READ.IIC_Rx_Buffer <<= AHT20_READ.IIC_Bit_Cnt; 
        GPIO_Set(PIN_SDA, ((AHT20_READ.IIC_Rx_Buffer) & 0x80)); 
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_SEND_ADDRESS_STEP2;
    }
    break;
        //等待3us，改变scl
    case READ_I2C_SEND_ADDRESS_STEP2 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SCL, 1);
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_SEND_ADDRESS_STEP3;
    }
    break;
        //保持scl高电平5us
    case READ_I2C_SEND_ADDRESS_STEP3 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        GPIO_Set(PIN_SCL, 0);
        AHT20_READ.Read_State = READ_I2C_SEND_ADDRESS_STEP4;
    }
    break;
       
    case READ_I2C_SEND_ADDRESS_STEP4 :
    AHT20_READ.IIC_Bit_Cnt++;
    if(AHT20_READ.IIC_Bit_Cnt == 8)
    {
        AHT20_READ.IIC_Bit_Cnt = 0;
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_Event = READ_I2C_EVENT_RECEIVE_ACK; // 进入接收ACK阶段
        AHT20_READ.Read_State = READ_I2C_RECEIVE_ACK_STEP1;
    }
    else
    {
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_SEND_ADDRESS_STEP1; // 回到改变SCL的步骤，继续发送下一位
    }
    break;
}
}

void Platform_Aht20_Standard_IIC_Read_Receive_Ack(void)
{
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
        //等待3us，释放sda（输入）
    case READ_I2C_RECEIVE_ACK_STEP1 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SDA, 1); // 释放SDA线，准备接收ACK
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RECEIVE_ACK_STEP2;
    }
    break;
        //等待3us，scl拉高
    case READ_I2C_RECEIVE_ACK_STEP2 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SCL, 1); // 拉高SCL线，准备读取ACK位
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RECEIVE_ACK_STEP3;
    }
    break;
        //保持scl高电平5us，读取ack
    case READ_I2C_RECEIVE_ACK_STEP3 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        AHT20_READ.IIC_Rx_Buffer_Long++;
        AHT20_READ.IIC_Bit_Cnt = 0; 
        AHT20_READ.ack = GPIO_Get(PIN_SDA); // 读取ACK位

        GPIO_Set(PIN_SCL, 0); // 拉低SCL线，结束ACK位的时钟周期
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;

        if(AHT20_READ.ack == 0)
        {
            if (AHT20_READ.IIC_Rx_Buffer_Long < 4) //注意这个pre是4
            {
                AHT20_READ.IIC_Rx_Adress++; // 移动到下一个地址
                AHT20_READ.Read_Event = READ_I2C_EVENT_ADDRESS; // 进入重复起始条件阶段
                AHT20_READ.Read_State = READ_I2C_SEND_ADDRESS_STEP1; // 重复起始条件的第一步
            }
            else
            {
                AHT20_READ.IIC_Rx_Buffer_Long = 0;
            AHT20_READ.Read_Event = READ_I2C_EVENT_STOP; // 停止通信
            AHT20_READ.Read_State = READ_I2C_STOP_STEP1; // 进入停止条件的第一步
            }

        }
        else
        {
            // ACK接收失败，可以选择重试或停止通信
            AHT20_READ.Read_Event = READ_I2C_EVENT_RE_STOP; // 停止通信
            AHT20_READ.Read_State = READ_I2C_RE_STOP_STEP1; // 进入停止条件的第一
        }
    }
    break;
}
}

void Platform_Aht20_Standard_IIC_Read_Stop(void)
{
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
        //停止条件：SCL为低电平，SDA由低变高
    case READ_I2C_STOP_STEP1 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        GPIO_Set(PIN_SDA, 0); // 确保SDA线为低电平
        GPIO_Set(PIN_SCL, 1); // 拉高SCL线，准备发送停止条件
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_STOP_STEP2;
    }
    break;
        //等待3us，改变sda
    case READ_I2C_STOP_STEP2 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SDA, 1); // 在SCL高电平期间将SDA线拉高，发送停止条件
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_STOP_STEP3;
    }
    break;
        //等待5us，完成停止条件
    case READ_I2C_STOP_STEP3 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
         now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_Event = READ_I2C_EVENT_WAIT; // 返回空闲状态
        AHT20_READ.Read_State = READ_I2C_WAIT_STEP1; // 准备下一次通信的起始条件
    }
    break;
}
}

void Platform_Aht20_Standard_IIC_Read_Wait(void)
{
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
    case READ_I2C_WAIT_STEP1 :
    if((now - AHT20_READ.IIC_Wait_Tick) >=80000) // 80ms，确保AHT20有足够时间完成测量并准备好数据
    {
        AHT20_READ.Read_Event = READ_I2C_EVENT_RE_START; // 返回空闲状态
        AHT20_READ.Read_State = READ_I2C_RE_START_STEP1; // 等待下一次通信的触发
    }
    break;
}
}

void Platform_Aht20_Standard_IIC_Read_Re_Start(void)
{
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
        //重复起始条件：SCL和SDA都为高电平
       case READ_I2C_RE_START_STEP1 :
    GPIO_Set(PIN_SDA, 1);
    GPIO_Set(PIN_SCL, 1);
     AHT20_READ.IIC_Wait_Tick =now ;
     AHT20_READ.Read_State = READ_I2C_RE_START_STEP2;
    break;
        //重复起始条件：等待5us，SDA在SCL高电平期间由高变低
      case READ_I2C_RE_START_STEP2 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        GPIO_Set(PIN_SDA, 0);
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RE_START_STEP3;
    }
    break;
   //重复起始条件：直接把scl为0
      case READ_I2C_RE_START_STEP3 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        GPIO_Set(PIN_SCL, 0);
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_Event = READ_I2C_EVENT_RE_ADDRESS; // 进入发送地址阶段
        AHT20_READ.Read_State = READ_I2C_RE_SEND_ADDRESS_STEP1;
    }
    break;
}
}

void Platform_Aht20_Standard_IIC_Read_Re_Send_Address(void)
{
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
        //等待3us，改变sda
    case READ_I2C_RE_SEND_ADDRESS_STEP1 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        AHT20_READ.IIC_Rx_Buffer = *(AHT20_READ.IIC_Rx_Re_Adress); 
        AHT20_READ.IIC_Rx_Buffer <<= AHT20_READ.IIC_Bit_Cnt; 
        GPIO_Set(PIN_SDA, ((AHT20_READ.IIC_Rx_Buffer) & 0x80)); 
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RE_SEND_ADDRESS_STEP2;
    }
    break;
        //等待3us，改变scl
    case READ_I2C_RE_SEND_ADDRESS_STEP2 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SCL, 1);
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RE_SEND_ADDRESS_STEP3;
    }
    break;
        //保持scl高电平5us
    case READ_I2C_RE_SEND_ADDRESS_STEP3 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        GPIO_Set(PIN_SCL, 0);
        AHT20_READ.Read_State = READ_I2C_RE_SEND_ADDRESS_STEP4;
    }
    break;
        
    case READ_I2C_RE_SEND_ADDRESS_STEP4 :
    AHT20_READ.IIC_Bit_Cnt++;
    if(AHT20_READ.IIC_Bit_Cnt == 8)
    {
        AHT20_READ.IIC_Bit_Cnt = 0;
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_Event = READ_I2C_EVENT_RE_RECEIVE_ACK; // 进入接收ACK阶段
        AHT20_READ.Read_State = READ_I2C_RE_RECEIVE_ACK_STEP1;
    }
    else
    {
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RE_SEND_ADDRESS_STEP1; // 回到改变SCL的步骤，继续发送下一位
    }
    break;
}
}

void Platform_Aht20_Standard_IIC_Read_Re_Receive_Ack(void)
{
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
        //等待3us，释放sda（输入）
    case READ_I2C_RE_RECEIVE_ACK_STEP1 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SDA, 1); // 释放SDA线，准备接收ACK
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RE_RECEIVE_ACK_STEP2;
    }
    break;
        //等待3us，scl拉高
    case READ_I2C_RE_RECEIVE_ACK_STEP2 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SCL, 1); // 拉高SCL线，准备读取ACK位
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RE_RECEIVE_ACK_STEP3;
    }
    break;
        //保持scl高电平5us，读取ack
    case READ_I2C_RE_RECEIVE_ACK_STEP3 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        AHT20_READ.ack = GPIO_Get(PIN_SDA); // 读取ACK位
        AHT20_READ.IIC_Bit_Cnt = 0;
        GPIO_Set(PIN_SCL, 0); // 拉低SCL线，结束ACK位的时钟周期
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        if(AHT20_READ.ack == 0)
        {
            AHT20_READ.Read_Event = READ_I2C_EVENT_RECEIVE_DATA; // 进入接收数据阶段
            AHT20_READ.Read_State = READ_I2C_RECEIVE_DATA_STEP0; // 接收数据的第一步
        }
        else
        {
            // ACK接收失败，可以选择重试或停止通信
            AHT20_READ.Read_Event = READ_I2C_EVENT_RE_STOP; // 停止通信
            AHT20_READ.Read_State = READ_I2C_RE_STOP_STEP1; //
        }
    }
    break;
}
}


 void Platform_Aht20_Standard_IIC_Read_Receive_Data(void)
 {
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
        //等待3us，准备接收数据
        case READ_I2C_RECEIVE_DATA_STEP0 :
        AHT20_READ.IIC_Rx_Buffer =0 ;
        AHT20_READ.IIC_Bit_Cnt = 0 ;
        AHT20_READ.Read_State = READ_I2C_RECEIVE_DATA_STEP1;
        break;

    case READ_I2C_RECEIVE_DATA_STEP1 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SDA, 1); // 释放SDA线，准备接收数据
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RECEIVE_DATA_STEP2;
    }
    break;
        //等待3us，scl拉高
    case READ_I2C_RECEIVE_DATA_STEP2 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SCL, 1); // 拉高SCL线，准备读取数据位
        AHT20_READ.IIC_Rx_Buffer <<= 1; // 根据当前位数调整数据位置
        AHT20_READ.IIC_Rx_Buffer |= GPIO_Get(PIN_SDA); // 将接收到的数据写入缓冲区
        AHT20_READ.IIC_Bit_Cnt++;
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RECEIVE_DATA_STEP3;
    }
    break;
        //保持scl高电平5us
    case READ_I2C_RECEIVE_DATA_STEP3 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        GPIO_Set(PIN_SCL, 0); // 拉低SCL线，结束当前数据位的时钟周期
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;       
        if(AHT20_READ.IIC_Bit_Cnt == 8) // 已接收8位数据
        {
            AHT20_READ.Read_Event = READ_I2C_EVENT_SEND_ACK; // 进入发送ACK阶段
            AHT20_READ.Read_State = READ_I2C_SEND_ACK_STEP0; // 发送ACK的第一步
            *(AHT20_READ.IIC_Rx_Data) = AHT20_READ.IIC_Rx_Buffer; // 将接收到的数据存储到指定地址
        }
        else
        {
            AHT20_READ.Read_State = READ_I2C_RECEIVE_DATA_STEP1; // 继续接收下一位数据
        }
        break;
 }
}
 }

  void Platform_Aht20_Standard_IIC_Read_Send_Ack(void)
{
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
        //等待3us，发送ACK
        case READ_I2C_SEND_ACK_STEP0 :
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;   
        AHT20_READ.IIC_Rx_Buffer_Long++;
        if(AHT20_READ.IIC_Rx_Buffer_Long == 7)
        {
            AHT20_READ.Read_Event = READ_I2C_EVENT_SEND_NACK_STEP; // 进入停止条件阶段
            AHT20_READ.Read_State = READ_I2C_SEND_NACK_STEP1; // 停止条件的第一步
        }
        else
        {
            AHT20_READ.Read_State = READ_I2C_SEND_ACK_STEP1; // 接收数据的第一步
        }
        break;

    case READ_I2C_SEND_ACK_STEP1 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SDA, 0); // 拉低SDA线，发送ACK
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_SEND_ACK_STEP2;
    }
    break;
        //等待3us，scl拉高
    case READ_I2C_SEND_ACK_STEP2 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SCL, 1); // 拉高SCL线，完成ACK位的时钟周期
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_SEND_ACK_STEP3;
    }
    break;
        //保持scl高电平5us
    case READ_I2C_SEND_ACK_STEP3 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        GPIO_Set(PIN_SCL, 0); // 拉低SCL线，结束ACK位的时钟周期
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        if (AHT20_READ.IIC_Rx_Buffer_Long < 7 ) 
        {
            AHT20_READ.Read_Event = READ_I2C_EVENT_RECEIVE_DATA; 
            AHT20_READ.Read_State = READ_I2C_RECEIVE_DATA_STEP0; 
            AHT20_READ.IIC_Rx_Data++;  
        }
    }
}
}

void Platform_Aht20_Standard_IIC_Read_Send_Nack(void)
{
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
        //等待3us，发送NACK
    case READ_I2C_SEND_NACK_STEP1 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SDA, 1); // 拉高SDA线，发送NACK
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_SEND_NACK_STEP2;
    }
    break;
        //等待3us，scl拉高
    case READ_I2C_SEND_NACK_STEP2 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SCL, 1); // 拉高SCL线，完成NACK位的时钟周期
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_SEND_NACK_STEP3;
    }
    break;
        //保持scl高电平5us
    case READ_I2C_SEND_NACK_STEP3 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        GPIO_Set(PIN_SCL, 0); // 拉低SCL线，结束NACK位的时钟周期
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_Event = READ_I2C_EVENT_RE_STOP; // 进入停止条件阶段
        AHT20_READ.Read_State = READ_I2C_RE_STOP_STEP1; // 停止条件的第一步
    }
    break;
}
}

void Platform_Aht20_Standard_IIC_Read_Re_Stop(void)
{
    now = time_get_us();
    switch(AHT20_READ.Read_State)
    {
      
    case READ_I2C_RE_STOP_STEP1 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
        GPIO_Set(PIN_SDA, 0); // 确保SDA线为低电平
        GPIO_Set(PIN_SCL, 1); // 拉高SCL线，准备发送停止条件
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RE_STOP_STEP2;
    }
    break;
        //等待3us，改变sda
    case READ_I2C_RE_STOP_STEP2 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_3US)
    {
        GPIO_Set(PIN_SDA, 1); // 在SCL高电平期间将SDA线拉高，发送停止条件
        now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.Read_State = READ_I2C_RE_STOP_STEP3;
    }
    break;
        //等待5us，完成停止条件
    case READ_I2C_RE_STOP_STEP3 :
    if((now - AHT20_READ.IIC_Wait_Tick) >= IIC_DELAY_5US)
    {
         now = time_get_us();
        AHT20_READ.IIC_Wait_Tick =now ;
        AHT20_READ.IIC_Rx_Buffer_Long = 0;
        AHT20_READ.IIC_Bit_Cnt = 0;
        AHT20_READ.Read_Event = READ_I2C_EVENT_IDLE; // 返回空闲状态
        AHT20_READ.Read_State = READ_I2C_START_STEP1; // 准备下一次通信的起始条件
         AHT20_READ.IIC_Read_Complete = 1; // 标记读操作完成
    }
    break;
}
}




void Platform_Aht20_Standard_IIC_Read_Task(void) 
{
    switch(AHT20_READ.Read_Event)
    {
        case  READ_I2C_EVENT_IDLE :   
          return ;
        break;

        case READ_I2C_EVENT_START:
        Platform_Aht20_Standard_IIC_Read_Start();
        break;

        case READ_I2C_EVENT_ADDRESS:
        Platform_Aht20_Standard_IIC_Read_Devidce_And_Word_Address();
        break;

         case READ_I2C_EVENT_RECEIVE_ACK:
        Platform_Aht20_Standard_IIC_Read_Receive_Ack();
        break;

        case READ_I2C_EVENT_STOP:
         Platform_Aht20_Standard_IIC_Read_Stop();
        break;

       case READ_I2C_EVENT_WAIT:
         Platform_Aht20_Standard_IIC_Read_Wait();
        break;

        case READ_I2C_EVENT_RE_START:
        Platform_Aht20_Standard_IIC_Read_Re_Start();
        break;

        case READ_I2C_EVENT_RE_ADDRESS:
        Platform_Aht20_Standard_IIC_Read_Re_Send_Address();
        break;

        case READ_I2C_EVENT_RE_RECEIVE_ACK:
        Platform_Aht20_Standard_IIC_Read_Re_Receive_Ack();
        break;

        case READ_I2C_EVENT_RECEIVE_DATA:
         Platform_Aht20_Standard_IIC_Read_Receive_Data();
        break;

        case READ_I2C_EVENT_SEND_ACK:
        Platform_Aht20_Standard_IIC_Read_Send_Ack();
        break;

        case READ_I2C_EVENT_SEND_NACK_STEP:
        Platform_Aht20_Standard_IIC_Read_Send_Nack();
        break;

        case READ_I2C_EVENT_RE_STOP:
        Platform_Aht20_Standard_IIC_Read_Re_Stop();
        break;
        


    }
}

unsigned char Platform_Aht20_Standard_IIC_Read_Complete_Flag(void) 
{
    return AHT20_READ.IIC_Read_Complete;
}

unsigned char Platform_Aht20_Standard_IIC_Read_Complete_Flag_Clear(void) 
{
    AHT20_READ.IIC_Read_Complete = 0;
    return AHT20_READ.IIC_Read_Complete;
}