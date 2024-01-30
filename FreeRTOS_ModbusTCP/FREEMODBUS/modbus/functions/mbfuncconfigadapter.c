/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"


eMBException    prveMBError2Exception( eMBErrorCode eErrorCode );

/*config adpter FUNCTION CODE = 0x09*/

#if MB_FUNC_CONFIG_ADAPTER_ENABLED > 0

eMBException 
eMBFuncConfigAdapter( UCHAR * pucFrame, USHORT * usLen ){
	    
	UCHAR          *pucFrameCur;//responce frame
  eMBException    eStatus = MB_EX_NONE;
  eMBErrorCode    eRegStatus;
	/*function code and data length eg:8 bytes data then uslen = 9*/
  if( *usLen == ( 9 ) )
  { 
		
		/* Set the current PDU data pointer to the beginning. */
    pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];//0
    *usLen = MB_PDU_FUNC_OFF;

    /* First byte contains the function code. */
    *pucFrameCur++ = MB_FUNC_CONFIG_ADAPTER;
    *usLen += 1;

		//callback function-->put configration data to eeprom
    eRegStatus = eMBEepromInputCB(&pucFrame[1], MB_REG_WRITE);
    if( eRegStatus != MB_ENOERR )
		{
			eStatus = prveMBError2Exception( eRegStatus );
		}
		else
		{
			*usLen += 4 * 2;
		}
  }
  else
  {
      /* Can't be a valid request because the length is incorrect. */
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}

#endif
/* ��վ����0x0b���������ô�վʱ�� */
eMBException
eMBFuncSetAdapterTime( UCHAR * pucFrame, USHORT * usLen ){
	    
	UCHAR          *pucFrameCur;//responce frame
  eMBException    eStatus = MB_EX_NONE;
  eMBErrorCode    eRegStatus;
	/*function code and data length eg:8 bytes data then uslen = 9*/
  if( *usLen == ( 7 ) )
  { 
		
		/* Set the current PDU data pointer to the beginning. */
    pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];//0
    *usLen = MB_PDU_FUNC_OFF;

    /* First byte contains the function code. */
    *pucFrameCur++ = MB_FUNC_SET_ADAPTER_TIME;
    *usLen += 1;

		//callback function-->put configration data to eeprom
    eRegStatus = eMBSetTime(pucFrameCur);
    if( eRegStatus != MB_ENOERR )
		{
			eStatus = prveMBError2Exception( eRegStatus );
		}
		else
		{
			*usLen += 3 * 2;
		}
	}
  
  else
  {
      /* Can't be a valid request because the length is incorrect. */
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}
/* ��վ����0x0c���������ô�վIP */
eMBException eMBFuncSetAdapterIP( UCHAR * pucFrame, USHORT * usLen ){

	UCHAR          *pucFrameCur;//responce frame
  eMBException    eStatus = MB_EX_NONE;
  eMBErrorCode    eRegStatus;
	/*function code and data length eg:8 bytes data then uslen = 9*/
  if( *usLen == ( 7 ) )
  { 
		
		/* Set the current PDU data pointer to the beginning. */
    pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];//0
    *usLen = MB_PDU_FUNC_OFF;

    /* First byte contains the function code. */
    *pucFrameCur++ = MB_FUNC_SET_ADAPTER_IP;
    *usLen += 1;

		//callback function-->put configration data to eeprom
    eRegStatus = eMBSetIP(pucFrameCur);
    if( eRegStatus != MB_ENOERR )
		{
			eStatus = prveMBError2Exception( eRegStatus );
		}
		else
		{
			*usLen += 3 * 2;
		}
	}
  
  else
  {
      /* Can't be a valid request because the length is incorrect. */
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}
/* ��վ����0x0d�����봥���ص����� */
eMBException eMBFuncCallBack( UCHAR * pucFrame, USHORT * usLen ){
	UCHAR          *pucFrameCur;//responce frame
  eMBException    eStatus = MB_EX_NONE;
  eMBErrorCode    eRegStatus;
	/*function code and data length eg:8 bytes data then uslen = 9*/
  if( *usLen == ( 2 ) )
  { 
		
		/* Set the current PDU data pointer to the beginning. */
    pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];//0
    *usLen = MB_PDU_FUNC_OFF;

    /* First byte contains the function code. */
    *pucFrameCur++ = MB_FUNC_CALL_BACK;
    *usLen += 1;

		//callback function-->put configration data to eeprom
    eRegStatus = eMBCallBack(pucFrameCur);
    if( eRegStatus != MB_ENOERR )
		{
			eStatus = prveMBError2Exception( eRegStatus );
		}
		else
		{
			*usLen += 1;
		}
	}
  
  else
  {
      /* Can't be a valid request because the length is incorrect. */
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;


}


#if MB_FUNC_READ_CONFIG_ADAPTER_ENABLED > 0

eMBException 
eMBFuncReadConfigAdapter( UCHAR * pucFrame, USHORT * usLen ){
	    
	UCHAR          *pucFrameCur;//responce frame
  eMBException    eStatus = MB_EX_NONE;
  eMBErrorCode    eRegStatus;
	/*function code and data length eg:8 bytes data then uslen = 9*/
  if( *usLen == ( 1 ) )
  { 
		
		/* Set the current PDU data pointer to the beginning. */
    pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];//0
    *usLen = MB_PDU_FUNC_OFF;

    /* First byte contains the function code. */
    *pucFrameCur++ = MB_FUNC_READ_CONFIG_ADAPTER;
    *usLen += 1;

		//callback function-->put configration data to eeprom
    eRegStatus = eMBEepromInputCB(pucFrameCur, MB_REG_READ);
    if( eRegStatus != MB_ENOERR )
		{
			eStatus = prveMBError2Exception( eRegStatus );
		}
		else
		{
			*usLen += 4 * 2;
		}
	}
  
  else
  {
      /* Can't be a valid request because the length is incorrect. */
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}

#endif
