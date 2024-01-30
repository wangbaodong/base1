#include "sht30.h"
#include "Simulate.h"
void Sht30_Init(void)
{	IIC2_Init();

	IIC2_Start();
	IIC2_Send_Byte(0x44<<1|0);//0为写
	IIC2_Wait_Ack();
	IIC2_Send_Byte(0x21);
	IIC2_Wait_Ack();
	IIC2_Send_Byte(0x30);
	IIC2_Wait_Ack();
	IIC2_Stop();
	HAL_Delay(150);
}

void Read_IIC_Data(float *Temperature,float *Humidity)
{
	uint16_t data[6];
	uint16_t temp,humi;
	IIC2_Start();
	IIC2_Send_Byte(0x44<<1|0);//0为写
	IIC2_Wait_Ack();
	IIC2_Send_Byte(0xe0);
	IIC2_Wait_Ack();
	IIC2_Send_Byte(0x00);
	IIC2_Wait_Ack();

	IIC2_Start();
	IIC2_Send_Byte(0x44<<1|1);
	IIC2_Wait_Ack();
//温度数据
	data[0]=IIC2_Read_Byte(1);
	data[1]=IIC2_Read_Byte(1);
//CRC
	data[2]=IIC2_Read_Byte(1);
//湿度数据
	data[3]=IIC2_Read_Byte(1);
	data[4]=IIC2_Read_Byte(1);

	data[5]=IIC2_Read_Byte(0);
	IIC2_Stop();
//校验和就不计算了
	temp=(data[0]<<8)|data[1];
	humi=(data[3]<<8)|data[4];
	
	/*转换实际温度*/
	*Temperature=(175.0*(float)temp/65535.0-45.0) ;// T = -45 + 175 * tem / (2^16-1)
	*Humidity=(100.0*(float)humi/65535.0);// RH = hum*100 / (2^16-1)

}
