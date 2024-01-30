#include "Simulate.h"
#include "gpio.h"
//??'??IIC???????
void IIC2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_Pin_SDA2|GPIO_Pin_SCL2;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD,&GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOD,GPIO_Pin_SDA2|GPIO_Pin_SCL2,GPIO_PIN_SET);						
}
void SDA2_OUT(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_Pin_SDA2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIO_Port_SDA2, &GPIO_InitStruct);
	
}
void SDA2_IN(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_Pin_SDA2;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIO_Port_SDA2, &GPIO_InitStruct);
	
}

//IIC??'?z?
void IIC2_Start(void)
{
	SDA2_OUT();
	
	IIC2_SDA_HIGH;
	IIC2_SCL_HIGH;
	delayUs(2);
	IIC2_SDA_LOW;
	delayUs(2);
	IIC2_SCL_LOW;
}

//IIC?????z?
void IIC2_Stop(void)
{
	SDA2_OUT();
	IIC2_SCL_LOW;
	IIC2_SDA_LOW;
	delayUs(2);
	IIC2_SCL_HIGH;
	delayUs(2);
	IIC2_SDA_HIGH;
	delayUs(2);
}

//????ACK????z?
void IIC2_Ack(void)
{
	IIC2_SCL_LOW;
	SDA2_OUT();
	IIC2_SDA_LOW;
	delayUs(2);
	IIC2_SCL_HIGH;
	delayUs(2);
	IIC2_SCL_LOW;
}
	
//??????ACK????z?
void IIC2_NAck(void)
{
	IIC2_SCL_LOW;
	SDA2_OUT();
	IIC2_SDA_HIGH;
	delayUs(2);
	IIC2_SCL_HIGH;
	delayUs(2);
	IIC2_SCL_LOW;
}
//???????zw???
//???????1????????????
//        0???????????
uint8_t IIC2_Wait_Ack(void)
{
	uint8_t Time=0;
	SDA2_IN();
	IIC2_SCL_HIGH;
	while(READ_SDA2)
	{
		Time++;
		if(Time>250)
		{
			IIC2_Stop();
			return 1;
		}
	}
	IIC2_SCL_LOW;
	return 0;
}
	
//IIC????h?????
void IIC2_Send_Byte(uint8_t txd)
{
	uint8_t t;
	SDA2_OUT();
	IIC2_SCL_LOW;
	for(t=0;t<8;t++)
	{
		HAL_GPIO_WritePin(GPIO_Port_SDA2,GPIO_Pin_SDA2,(txd&0x80)>>7);
//		IIC_SDA=(txd&0x80)>>7;
		txd<<=1;
		IIC2_SCL_HIGH;
		delayUs(2);
		IIC2_SCL_LOW;
		delayUs(2);
	}
}

//IIC??h?????
//ack=1 ????ACK
//ack=0 ????NACK
uint8_t IIC2_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	SDA2_IN();
	for(i=0;i<8;i++)
	{
		IIC2_SCL_LOW;
		delayUs(2);
		IIC2_SCL_HIGH;
		receive<<=1;
		if(READ_SDA2)receive++;
		delayUs(2);
	}
	if(!ack) IIC2_NAck();
	else IIC2_Ack();
	return receive;
}
