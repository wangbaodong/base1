#ifndef MPU6050_H
#define MPU6050_H
#include "main.h"


//?J??????????
#define MPU_SELF_TESTX_REG		0X0D	//???J???X
#define MPU_SELF_TESTY_REG		0X0E	//???J???Y
#define MPU_SELF_TESTZ_REG		0X0F	//???J???Z
#define MPU_SELF_TESTA_REG		0X10	//???J???A
#define MPU_SAMPLE_RATE_REG		0X19	//???????????
#define MPU_CFG_REG				0X1A	//???�J???
#define MPU_GYRO_CFG_REG		0X1B	//?????????�J???
#define MPU_ACCEL_CFG_REG		0X1C	//?????????�J???
#define MPU_MOTION_DET_REG		0X1F	//??????????�J???
#define MPU_FIFO_EN_REG			0X23	//FIFO'??J???
#define MPU_I2CMST_CTRL_REG		0X24	//IIC????????J???
#define MPU_I2CSLV0_ADDR_REG	0X25	//IIC???0????????J???
#define MPU_I2CSLV0_REG			0X26	//IIC???0???????J???
#define MPU_I2CSLV0_CTRL_REG	0X27	//IIC???0????J???
#define MPU_I2CSLV1_ADDR_REG	0X28	//IIC???1????????J???
#define MPU_I2CSLV1_REG			0X29	//IIC???1???????J???
#define MPU_I2CSLV1_CTRL_REG	0X2A	//IIC???1????J???
#define MPU_I2CSLV2_ADDR_REG	0X2B	//IIC???2????????J???
#define MPU_I2CSLV2_REG			0X2C	//IIC???2???????J???
#define MPU_I2CSLV2_CTRL_REG	0X2D	//IIC???2????J???
#define MPU_I2CSLV3_ADDR_REG	0X2E	//IIC???3????????J???
#define MPU_I2CSLV3_REG			0X2F	//IIC???3???????J???
#define MPU_I2CSLV3_CTRL_REG	0X30	//IIC???3????J???
#define MPU_I2CSLV4_ADDR_REG	0X31	//IIC???4????????J???
#define MPU_I2CSLV4_REG			0X32	//IIC???4???????J???
#define MPU_I2CSLV4_DO_REG		0X33	//IIC???4?????J???
#define MPU_I2CSLV4_CTRL_REG	0X34	//IIC???4????J???
#define MPU_I2CSLV4_DI_REG		0X35	//IIC???4??????J???

#define MPU_I2CMST_STA_REG		0X36	//IIC???????J???
#define MPU_INTBP_CFG_REG		0X37	//???/??�???�J???
#define MPU_INT_EN_REG			0X38	//???'??J???
#define MPU_INT_STA_REG			0X3A	//??????J???

#define MPU_ACCEL_XOUTH_REG		0X3B	//??????,X???8??J???
#define MPU_ACCEL_XOUTL_REG		0X3C	//??????,X???8??J???
#define MPU_ACCEL_YOUTH_REG		0X3D	//??????,Y???8??J???
#define MPU_ACCEL_YOUTL_REG		0X3E	//??????,Y???8??J???
#define MPU_ACCEL_ZOUTH_REG		0X3F	//??????,Z???8??J???
#define MPU_ACCEL_ZOUTL_REG		0X40	//??????,Z???8??J???

#define MPU_TEMP_OUTH_REG		0X41	//?�???????J???
#define MPU_TEMP_OUTL_REG		0X42	//?�????8??J???

#define MPU_GYRO_XOUTH_REG		0X43	//???????,X???8??J???
#define MPU_GYRO_XOUTL_REG		0X44	//???????,X???8??J???
#define MPU_GYRO_YOUTH_REG		0X45	//???????,Y???8??J???
#define MPU_GYRO_YOUTL_REG		0X46	//???????,Y???8??J???
#define MPU_GYRO_ZOUTH_REG		0X47	//???????,Z???8??J???
#define MPU_GYRO_ZOUTL_REG		0X48	//???????,Z???8??J???

#define MPU_I2CSLV0_DO_REG		0X63	//IIC???0????J???
#define MPU_I2CSLV1_DO_REG		0X64	//IIC???1????J???
#define MPU_I2CSLV2_DO_REG		0X65	//IIC???2????J???
#define MPU_I2CSLV3_DO_REG		0X66	//IIC???3????J???

#define MPU_I2CMST_DELAY_REG	0X67	//IIC????????????J???
#define MPU_SIGPATH_RST_REG		0X68	//?z????????J???
#define MPU_MDETECT_CTRL_REG	0X69	//?????????J???
#define MPU_USER_CTRL_REG		0X6A	//?�?????J???
#define MPU_PWR_MGMT1_REG		0X6B	//????????J???1
#define MPU_PWR_MGMT2_REG		0X6C	//????????J???2 
#define MPU_FIFO_CNTH_REG		0X72	//FIFO?????J???????
#define MPU_FIFO_CNTL_REG		0X73	//FIFO?????J???????
#define MPU_FIFO_RW_REG			0X74	//FIFO????J???
#define MPU_DEVICE_ID_REG		0X75	//????ID?J???

//?? MPU_AD0 ????/?? GND ?????g??? IIC ????????? 0X68??
//?? MPU_AD0 ?? VCC?????g??? IIC ????????? 0X69
#define MPU_ADDR				0X68

uint8_t MPU6050_Init(void); 								//??'??MPU6050
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr);//????MPU6050???????????????????
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr);//????MPU6050??????????????????
uint8_t MPU_Set_LPF(uint16_t lpf);//????MPU6050?????????????
uint8_t MPU_Set_Rate(uint16_t rate);//????MPU6050???????(???Fs=1KHz)
uint8_t MPU_Set_Rate(uint16_t rate);//?�??�??

uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data);//IIC?h?????
uint8_t MPU_Read_Byte(uint8_t reg);//IIC??h????? 
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);//IIC?????
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);//IIC??????
uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz);
uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az);
short MPU_Get_Temperature(void);



#endif
