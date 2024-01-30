#include "Simulate.h"
#include "gpio.h"
//??'??IIC???????
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_Pin_SDA|GPIO_Pin_SCL;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOC,GPIO_Pin_SDA|GPIO_Pin_SCL,GPIO_PIN_SET);						
}
void SDA_OUT(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_Pin_SDA;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIO_Port_SDA, &GPIO_InitStruct);
	
}
void SDA_IN(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_Pin_SDA;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIO_Port_SDA, &GPIO_InitStruct);
	
}

//IIC??'?z?
void IIC_Start(void)
{
	SDA_OUT();
	
	IIC_SDA_HIGH;
	IIC_SCL_HIGH;
	delayUs2(2);
	IIC_SDA_LOW;
	delayUs2(2);
	IIC_SCL_LOW;
}

//IIC?????z?
void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL_LOW;
	IIC_SDA_LOW;
	delayUs2(2);
	IIC_SCL_HIGH;
	IIC_SDA_HIGH;
	delayUs2(2);
}

//????ACK????z?
void IIC_Ack(void)
{
	IIC_SCL_LOW;
	SDA_OUT();
	IIC_SDA_LOW;
	delayUs2(2);
	IIC_SCL_HIGH;
	delayUs2(2);
	IIC_SCL_LOW;
}
	
//??????ACK????z?
void IIC_NAck(void)
{
	IIC_SCL_LOW;
	SDA_OUT();
	IIC_SDA_HIGH;
	delayUs2(2);
	IIC_SCL_HIGH;
	delayUs2(2);
	IIC_SCL_LOW;
}
//???????zw???
//???????1????????????
//        0???????????
uint8_t IIC_Wait_Ack(void)
{
	uint8_t Time=0;
	SDA_IN();
	IIC_SDA_HIGH;
	delayUs2(2);
	IIC_SCL_HIGH;
	delayUs2(2);
	while(READ_SDA)
	{
		Time++;
		if(Time>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_LOW;
	return 0;
}
	
//IIC????h?????
void IIC_Send_Byte(uint8_t txd)
{
	uint8_t t;
	SDA_OUT();
	IIC_SCL_LOW;
	for(t=0;t<8;t++)
	{
		HAL_GPIO_WritePin(GPIO_Port_SDA,GPIO_Pin_SDA,(txd&0x80)>>7);
//		IIC_SDA=(txd&0x80)>>7;
		txd<<=1;
		IIC_SCL_HIGH;
		delayUs2(2);
		IIC_SCL_LOW;
		delayUs2(2);
	}
}

//IIC??h?????
//ack=1 ????ACK
//ack=0 ????NACK
uint8_t IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	SDA_IN();
	for(i=0;i<8;i++)
	{
		IIC_SCL_LOW;
		delayUs2(2);
		IIC_SCL_HIGH;
		receive<<=1;
		if(READ_SDA)receive++;
		delayUs2(2);
	}
	if(!ack) IIC_NAck();
	else IIC_Ack();
	return receive;
}
