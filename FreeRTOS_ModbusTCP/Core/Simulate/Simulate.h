#ifndef SIMULATE_H
#define SIMULATE_H

#include "main.h"


#define GPIO_Pin_SDA GPIO_PIN_8
#define GPIO_Pin_SCL GPIO_PIN_6
#define GPIO_Port_SDA GPIOA
#define GPIO_Port_SCL GPIOC

//IO????????
//#define SDA_IN() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=8<<4;}
//#define SDA_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|3<<4;}
//#define SDA_IN() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
//#define SDA_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}
void	SDA_OUT(void);
void	SDA_IN(void);

//IO????????
//#define IIC_SCL PBout(8)
//#define IIC_SDA PBout(9)
//#define READ_SDA PBin(9)
//#define IIC_SCL PCout(12)
//#define IIC_SDA PCout(11)
#define READ_SDA HAL_GPIO_ReadPin(GPIO_Port_SDA,GPIO_Pin_SDA)
#define IIC_SCL_HIGH HAL_GPIO_WritePin(GPIO_Port_SCL,GPIO_Pin_SCL,GPIO_PIN_SET)
#define IIC_SCL_LOW HAL_GPIO_WritePin(GPIO_Port_SCL,GPIO_Pin_SCL,GPIO_PIN_RESET)
#define IIC_SDA_HIGH HAL_GPIO_WritePin(GPIO_Port_SDA,GPIO_Pin_SDA,GPIO_PIN_SET)
#define IIC_SDA_LOW HAL_GPIO_WritePin(GPIO_Port_SDA,GPIO_Pin_SDA,GPIO_PIN_RESET)

//IIC????????
void IIC_Init(void);//??'??IIC??IO??
void IIC_Start(void); //IIC??'?z?
void IIC_Stop(void);//IIC?????z?
void IIC_Ack(void);//????ACK????z?
void IIC_NAck(void);//??????ACK????z?
uint8_t IIC_Wait_Ack(void); //???????z?
void IIC_Send_Byte(uint8_t txd);//IIC????h?????
uint8_t IIC_Read_Byte(uint8_t ack);//IIC??h?????




/* iic2 */
#define GPIO_Pin_SDA2 GPIO_PIN_8
#define GPIO_Pin_SCL2 GPIO_PIN_10
#define GPIO_Port_SDA2 GPIOD
#define GPIO_Port_SCL2 GPIOD
#define READ_SDA2 HAL_GPIO_ReadPin(GPIO_Port_SDA2,GPIO_Pin_SDA2)
#define IIC2_SCL_HIGH HAL_GPIO_WritePin(GPIO_Port_SCL2,GPIO_Pin_SCL2,GPIO_PIN_SET)
#define IIC2_SCL_LOW HAL_GPIO_WritePin(GPIO_Port_SCL2,GPIO_Pin_SCL2,GPIO_PIN_RESET)
#define IIC2_SDA_HIGH HAL_GPIO_WritePin(GPIO_Port_SDA2,GPIO_Pin_SDA2,GPIO_PIN_SET)
#define IIC2_SDA_LOW HAL_GPIO_WritePin(GPIO_Port_SDA2,GPIO_Pin_SDA2,GPIO_PIN_RESET)
void	SDA2_OUT(void);
void	SDA2_IN(void);
void IIC2_Init(void);//??'??IIC??IO??
void IIC2_Start(void); //IIC??'?z?
void IIC2_Stop(void);//IIC?????z?
void IIC2_Ack(void);//????ACK????z?
void IIC2_NAck(void);//??????ACK????z?
uint8_t IIC2_Wait_Ack(void); //???????z?
void IIC2_Send_Byte(uint8_t txd);//IIC????h?????
uint8_t IIC2_Read_Byte(uint8_t ack);//IIC??h?????
#endif
