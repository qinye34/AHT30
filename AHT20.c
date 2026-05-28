#include "AHT20.h"

void DRIVE_Aht20_Standard_IIC_Read_Address_Change(unsigned char *adress,unsigned char *re_adress,unsigned char *dat)reentrant
{
    Platform_Aht20_Standard_IIC_Read_Address(adress,re_adress,dat);
}

void DRIVE_Aht20_Standard_IIC_Read_Task_Change(void)
{
    Platform_Aht20_Standard_IIC_Read_Task();
}

AHT20_TypeDef Aht20 = {DRIVE_Aht20_Standard_IIC_Read_Address_Change,DRIVE_Aht20_Standard_IIC_Read_Task_Change} ;

void APPLICATION_Aht20_Standard_IIC_Read_Address(unsigned char *adress,unsigned char *re_adress,unsigned char *dat)
{
    Aht20.DRIVE_Aht20_Standard_IIC_Read_Address(adress,re_adress,dat);
}

void APPLICATION_Aht20_Standard_IIC_Read_Task(void)
{
    Aht20.DRIVE_Aht20_Standard_IIC_Read_Task();
}