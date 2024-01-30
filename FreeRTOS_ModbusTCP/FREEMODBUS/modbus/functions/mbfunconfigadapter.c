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

/*config adpter FUNCTION CODE = 0x09*/

#if MB_FUNC_CONFIG_ADAPTER_ENABLED > 0

eMBException 
eMBFuncConfigAdapter( UCHAR * pucFrame, USHORT * usLen ){
	
  USHORT          usRegAddress;
  USHORT          usRegCount;
  UCHAR          *pucFrameCur;

  eMBException    eStatus = MB_EX_NONE;
  eMBErrorCode    eRegStatus;

  if( *usLen == ( 1 + MB_PDU_SIZE_MIN ) )
  {
      usRegAddress = ( USHORT )( pucFrame[1] << 8 );
      usRegAddress |= ( USHORT )( pucFrame[1 + 1] );
      usRegAddress++;

      usRegCount = ( USHORT )( pucFrame[1] << 8 );
      usRegCount = ( USHORT )( pucFrame[1 + 1] );

      /* Check if the number of registers to read is valid. If not
       * return Modbus illegal data value exception. 
       */
      if( ( usRegCount >= 1 ) && ( usRegCount <= 1 ) )
      {
          /* Set the current PDU data pointer to the beginning. */
          pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
          *usLen = MB_PDU_FUNC_OFF;

          /* First byte contains the function code. */
          *pucFrameCur++ = MB_FUNC_READ_HOLDING_REGISTER;
          *usLen += 1;

          /* Second byte in the response contain the number of bytes. */
          *pucFrameCur++ = ( UCHAR ) ( usRegCount * 2 );
          *usLen += 1;

          /* Make callback to fill the buffer. */
          eRegStatus = eMBRegHoldingCB( pucFrameCur, usRegAddress, usRegCount, MB_REG_READ );
          /* If an error occured convert it into a Modbus exception. */
          if( eRegStatus != MB_ENOERR )
          {
              eStatus = prveMBError2Exception( eRegStatus );
          }
          else
          {
              *usLen += usRegCount * 2;
          }
      }
      else
      {
          eStatus = MB_EX_ILLEGAL_DATA_VALUE;
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