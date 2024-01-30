 /*
  * FreeModbus Libary: LPC214X Port
  * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  *
  * File: $Id: port.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
  */

/* ----------------------- System includes --------------------------------*/


/* ----------------------- Modbus includes ----------------------------------*/

#include "port.h"
#include "mb.h"
#include "mbport.h"
#include "at24c02.h"
#include "rtc.h"

/* ----------------------- Variables ----------------------------------------*/

extern uint8_t temp_Threshold;		
extern uint8_t hum_Threshold;
/* ----------------------- Start implementation -----------------------------*/
void
EnterCriticalSection(  )
{
	__ASM volatile("cpsid i");
}

void
ExitCriticalSection(  )
{
    __ASM volatile("cpsie i");
}


/******************************************************************************
                               ����Modbus��ز���
******************************************************************************/

uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff};
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS]={0x0002,0x0204,0x0306,0x0708,0x1122,0x3344,0x5566,0x7799,0x1234,0x2345};
uint8_t ucRegCoilsBuf[REG_COILS_SIZE]={0x3,0x2,0x3,0x4,0x5,0x6,0x7,0x8};
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
/**
  *****************************************************************************
  * @Name   : ��Adapter��������Ϣд��EEPROM
  *
  * @Brief  : ��Ӧ������0x09 -> eMBFuncConfigAdapter
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/

eMBErrorCode eMBEepromInputCB( UCHAR * pucRegBuffer, eMBRegisterMode eMode )
{	uint8_t I2c_Buf_Write[8];
	eMBErrorCode eStatus = MB_ENOERR;


	switch(eMode){
		case MB_REG_WRITE:
				for(int i=0;i<8;i++){
					I2c_Buf_Write[i]=*pucRegBuffer++;
				}
			  I2C_EE_BufferWrite(I2c_Buf_Write, 0x00, 8);
				temp_Threshold = I2c_Buf_Write[0];
				hum_Threshold  = I2c_Buf_Write[1];
		break;
		case MB_REG_READ:
				I2C_EE_BufferRead(I2c_Buf_Write,0x00,8);
				for(int i=0;i<8;i++){
					*pucRegBuffer++ = I2c_Buf_Write[i];
				}
		break;
	
	}

	return eStatus;
}

eMBErrorCode eMBSetTime( UCHAR * pucRegBuffer )
{
	eMBErrorCode eStatus = MB_ENOERR;
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	year = *pucRegBuffer++;
	month = *pucRegBuffer++;
	day = *pucRegBuffer++;
	hour = *pucRegBuffer++;
	minute = *pucRegBuffer++;
	second = *pucRegBuffer++;
	MX_RTC_Init(year,month,day,hour,minute,second);
  HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0, 0x32F2);


	return eStatus;
}

eMBErrorCode eMBSetIP( UCHAR * pucRegBuffer )
{
	eMBErrorCode eStatus = MB_ENOERR;
	uint8_t I2c_Buf_Write[32];
	I2C_EE_BufferRead(I2c_Buf_Write,0x00,32);

	for(int i=0;i<6;i++){
		I2c_Buf_Write[i+16]=*pucRegBuffer++;
	}
	I2C_EE_BufferWrite(I2c_Buf_Write,0x00,32);


	return eStatus;

}

eMBErrorCode eMBCallBack( UCHAR * pucRegBuffer )
{
	eMBErrorCode eStatus = MB_ENOERR;
	Handle(* pucRegBuffer,sdk_MBCallBack);
	return eStatus;

}

__weak void sdk_MBCallBack(int value){


}
/**
  *****************************************************************************
  * @Name   : ��������Ĵ���
  *
  * @Brief  : ��Ӧ������0x04 -> eMBFuncReadInputRegister
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;
	
	//
	//�жϵ�ַ�Ϸ���
	//
	if ((usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
	{
		iRegIndex = (int)(usAddress - REG_INPUT_START);
		while (usNRegs > 0)
		{
			*pucRegBuffer++ = (UCHAR)( usRegInputBuf[iRegIndex] >> 8);  //��8λ�ֽ�
			*pucRegBuffer++ = (UCHAR)( usRegInputBuf[iRegIndex] & 0xFF); //��8λ�ֽ�
			iRegIndex++;
			usNRegs--;
		}
	}
	else  //�����ַ
	{
		eStatus = MB_ENOREG;
	}
	
	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : �������ּĴ���
  *
  * @Brief  : ��Ӧ������0x06 -> eMBFuncWriteHoldingRegister
  *                    0x16 -> eMBFuncWriteMultipleHoldingRegister
  *                    0x03 -> eMBFuncReadHoldingRegister
  *                    0x23 -> eMBFuncReadWriteMultipleHoldingRegister
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *           eMode:         ������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;
	
	//
	//�жϵ�ַ�Ƿ�Ϸ�
	//
	if((usAddress >= REG_HOLDING_START) && ((usAddress + usNRegs) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - REG_HOLDING_START);
		//
		//���ݹ�������в���
		//
		switch(eMode)
		{
			case MB_REG_READ:  //�����ּĴ���
					while(usNRegs > 0)
					{
						*pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] >> 8);  //��8λ�ֽ�
						*pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] & 0xFF); //��8λ�ֽ�
						iRegIndex++;
						usNRegs--;
					}                            
					break;
					
			case MB_REG_WRITE:  //д���ּĴ���
					while(usNRegs > 0)
					{
						usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;  //��8λ�ֽ�
						usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;  //��8λ�ֽ�
						iRegIndex++;
						usNRegs--;
					}
					break;
		}
	}
	else  //�����ַ
	{
		eStatus = MB_ENOREG;
	}
	
	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : ������Ȧ
  *
  * @Brief  : ��Ӧ������0x01 -> eMBFuncReadCoils
  *                    0x05 -> eMBFuncWriteCoil
  *                    0x15 -> д�����Ȧ eMBFuncWriteMultipleCoils
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *           eMode:         ������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;
	
	//
	//�жϵ�ַ�Ϸ���
	//
	if ((usAddress >= REG_HOLDING_START) && ((usAddress + usNCoils) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - REG_HOLDING_START);
		//
		//���ݹ��������
		//
		switch (eMode)
		{
			case MB_REG_READ:  //��ȡ����
				while (usNCoils > 0)
				{
					*pucRegBuffer++ = (uint8_t)(ucRegCoilsBuf[iRegIndex] >> 8);  //��8λ�ֽ�
					*pucRegBuffer++ = (uint8_t)(ucRegCoilsBuf[iRegIndex] & 0xFF);  //��8λ�ֽ�
					iRegIndex++;
					usNCoils--;
				}
				break;
				
			case MB_REG_WRITE:  //д����
				while (usNCoils > 0)
				{
					ucRegCoilsBuf[iRegIndex] = *pucRegBuffer++ << 8;  //��8λ�ֽ�
					ucRegCoilsBuf[iRegIndex] |= *pucRegBuffer++;  //��8λ�ֽ�
					iRegIndex++;
					usNCoils--;
				}
				break;
		}
	}
	else  //�����ַ
	{
		eStatus = MB_ENOREG;
	}
	
	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : ������ɢ�Ĵ���
  *
  * @Brief  : ��Ӧ������0x02 -> eMBFuncReadDiscreteInputs
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	pucRegBuffer = pucRegBuffer;
	
	return MB_ENOREG;
}

void __aeabi_assert(const char * x1, const char * x2, int x3){

}
