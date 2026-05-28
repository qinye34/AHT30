#ifndef __PLATFORM_H__  // 如果__LCD1602_H__未定义
#define __PLATFORM_H__  // 定义这个宏，标记已包含

#include <reg52.h>



typedef enum {
    PIN_SCL ,  // 对应 P1.0
    PIN_SDA ,  // 对应 P1.1
    PIN_BEEP   ,  // 对应 P1.5	
  
} Pin_ID_TypeDef;


// 读操作专用I2C状态枚举
typedef enum
{
    READ_I2C_IDLE,
    READ_I2C_START_STEP1,
    READ_I2C_START_STEP2,
    READ_I2C_START_STEP3,


    READ_I2C_SEND_ADDRESS_STEP1,
    READ_I2C_SEND_ADDRESS_STEP2,
    READ_I2C_SEND_ADDRESS_STEP3,
    READ_I2C_SEND_ADDRESS_STEP4,

    READ_I2C_RECEIVE_ACK_STEP1,
    READ_I2C_RECEIVE_ACK_STEP2,
    READ_I2C_RECEIVE_ACK_STEP3,
    
READ_I2C_WAIT_STEP1,


    READ_I2C_RE_START_STEP1,
    READ_I2C_RE_START_STEP2,
    READ_I2C_RE_START_STEP3,
    READ_I2C_RE_START_STEP4,
    READ_I2C_RE_START_STEP5,

    READ_I2C_RE_SEND_ADDRESS_STEP1,
    READ_I2C_RE_SEND_ADDRESS_STEP2,
    READ_I2C_RE_SEND_ADDRESS_STEP3,
    READ_I2C_RE_SEND_ADDRESS_STEP4,
    READ_I2C_RE_SEND_ADDRESS_STEP5,
    
    READ_I2C_RE_RECEIVE_ACK_STEP1,
    READ_I2C_RE_RECEIVE_ACK_STEP2,
    READ_I2C_RE_RECEIVE_ACK_STEP3,
    
    READ_I2C_RECEIVE_DATA_STEP0,
    READ_I2C_RECEIVE_DATA_STEP1,
    READ_I2C_RECEIVE_DATA_STEP2,
    READ_I2C_RECEIVE_DATA_STEP3,

    READ_I2C_SEND_ACK_STEP0,
    READ_I2C_SEND_ACK_STEP1,
    READ_I2C_SEND_ACK_STEP2,
    READ_I2C_SEND_ACK_STEP3,
    READ_I2C_SEND_ACK_STEP4,
    READ_I2C_SEND_ACK_STEP5,

    READ_I2C_SEND_NACK_STEP1,
    READ_I2C_SEND_NACK_STEP2,
    READ_I2C_SEND_NACK_STEP3,
    READ_I2C_SEND_NACK_STEP4,

    READ_I2C_STOP_STEP1,
    READ_I2C_STOP_STEP2,
    READ_I2C_STOP_STEP3,

    READ_I2C_RE_STOP_STEP1,
    READ_I2C_RE_STOP_STEP2,
    READ_I2C_RE_STOP_STEP3

} Aht20_I2C_Read_State_Typedef;

typedef enum {
    READ_I2C_EVENT_IDLE,                 // 空闲
    READ_I2C_EVENT_START,          // 起始条件
    READ_I2C_EVENT_ADDRESS,        // 发送地址
    READ_I2C_EVENT_RECEIVE_ACK,    // 接收应答
    READ_I2C_EVENT_STOP,           // 停止条件
     READ_I2C_EVENT_WAIT,
    READ_I2C_EVENT_RE_START,       // 重复起始
    READ_I2C_EVENT_RE_ADDRESS,     // 重复起始后的地址
    READ_I2C_EVENT_RE_RECEIVE_ACK, // 重复起始后的应答
    READ_I2C_EVENT_RECEIVE_DATA,   // 接收数据
    READ_I2C_EVENT_RECEIVE_CRC,    // 接收CRC（AHT20无CRC，可忽略）
    READ_I2C_EVENT_SEND_ACK,       // 发送应答
    READ_I2C_EVENT_SEND_NACK_STEP, // 发送非应答
    READ_I2C_EVENT_RE_STOP         // 重复起始后的停止
} Aht20_I2C_Read_Event_Typedef;



void GPIO_Set(Pin_ID_TypeDef pin, unsigned char state);
unsigned char GPIO_Get(Pin_ID_TypeDef pin);

void Timer0_SysTick_Init(void); // 定时器0 模式1
unsigned long time_get_us(void); 

void delay_ms(unsigned int ms);

void Platform_UART_Init(void);
void Platform_UART_Send_Str(unsigned char *str, unsigned char len);

void Platform_Aht20_Standard_IIC_Read_Address(unsigned char *adress,unsigned char *re_adress,unsigned char *dat);
void Platform_Aht20_Standard_IIC_Read_Start(void);
void Platform_Aht20_Standard_IIC_Read_Devidce_And_Word_Address(void);
void Platform_Aht20_Standard_IIC_Read_Receive_Ack(void);
void Platform_Aht20_Standard_IIC_Read_Stop(void);
void Platform_Aht20_Standard_IIC_Read_Wait(void);
void Platform_Aht20_Standard_IIC_Read_Re_Start(void);
void Platform_Aht20_Standard_IIC_Read_Re_Send_Address(void);
void Platform_Aht20_Standard_IIC_Read_Re_Receive_Ack(void);
void Platform_Aht20_Standard_IIC_Read_Receive_Data(void);
void Platform_Aht20_Standard_IIC_Read_Send_Ack(void);
void Platform_Aht20_Standard_IIC_Read_Send_Nack(void);
void Platform_Aht20_Standard_IIC_Read_Re_Stop(void);
void Platform_Aht20_Standard_IIC_Read_Task(void) ;
unsigned char Platform_Aht20_Standard_IIC_Read_Complete_Flag(void) ;
unsigned char Platform_Aht20_Standard_IIC_Read_Complete_Flag_Clear(void) ;

#endif





