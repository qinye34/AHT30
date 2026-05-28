#include "PLATFORM.h"

typedef struct 
{
   void(*DRIVE_Aht20_Standard_IIC_Read_Address)(unsigned char *adress,unsigned char *re_adress,unsigned char *dat)reentrant;
   void(*DRIVE_Aht20_Standard_IIC_Read_Task)(void);
} AHT20_TypeDef;

void APPLICATION_Aht20_Standard_IIC_Read_Address(unsigned char *adress,unsigned char *re_adress,unsigned char *dat);
void APPLICATION_Aht20_Standard_IIC_Read_Task(void);