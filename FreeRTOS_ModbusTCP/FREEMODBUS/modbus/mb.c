/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mb.c,v 1.28 2010/06/06 13:54:40 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "sensorAdapter_sdk.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"
#include "mbport.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#if MB_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_ASCII_ENABLED == 1
#include "mbascii.h"
#endif
#if MB_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif

/* ----------------------- Static variables ---------------------------------*/

static UCHAR    ucMBAddress;
static UCHAR    ucMBTCPAddress;

static eMBMode  eMBCurrentMode;

static enum 
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
}eMBRTUSlaveState = STATE_NOT_INITIALIZED,eMBTCPState = STATE_NOT_INITIALIZED; 

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 */
static peMBFrameSend peMBFrameSendCur;
static pvMBFrameStart pvMBFrameStartCur;
static pvMBFrameStop pvMBFrameStopCur;
static peMBFrameReceive peMBFrameReceiveCur;
static pvMBFrameClose pvMBFrameCloseCur;


/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 */
BOOL( *pxMBFrameCBByteReceived ) ( void );
BOOL( *pxMBFrameCBTransmitterEmpty ) ( void );
BOOL( *pxMBPortCBTimerExpired ) ( void );

BOOL( *pxMBFrameCBReceiveFSMCur ) ( void );
BOOL( *pxMBFrameCBTransmitFSMCur ) ( void );

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
static xMBFunctionHandler xFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBFuncReadDiscreteInputs},
#endif
#if MB_FUNC_CONFIG_ADAPTER_ENABLED > 0
		{MB_FUNC_CONFIG_ADAPTER, eMBFuncConfigAdapter},
#endif
#if MB_FUNC_READ_CONFIG_ADAPTER_ENABLED > 0
		{MB_FUNC_READ_CONFIG_ADAPTER, eMBFuncReadConfigAdapter},
#endif
		{MB_FUNC_SET_ADAPTER_TIME, eMBFuncSetAdapterTime},
		{MB_FUNC_SET_ADAPTER_IP, eMBFuncSetAdapterIP},
		{MB_FUNC_CALL_BACK, eMBFuncCallBack},

};

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBInit( eMBMode eMode, UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    /* check preconditions */
    if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
        ( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
    {
        eStatus = MB_EINVAL;
    }
    else
    {
        ucMBAddress = ucSlaveAddress;

        switch ( eMode )
        {
#if MB_RTU_ENABLED > 0
        case MB_RTU:
            pvMBFrameStartCur = eMBRTUStart;
            pvMBFrameStopCur = eMBRTUStop;
            peMBFrameSendCur = eMBRTUSend;
            peMBFrameReceiveCur = eMBRTUReceive;
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
            pxMBFrameCBByteReceived = xMBRTUReceiveFSM;
            pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM;
            pxMBPortCBTimerExpired = xMBRTUTimerT35Expired;

            eStatus = eMBRTUInit( ucMBAddress, ucPort, ulBaudRate, eParity );
            break;
#endif
#if MB_ASCII_ENABLED > 0
        case MB_ASCII:
            pvMBFrameStartCur = eMBASCIIStart;
            pvMBFrameStopCur = eMBASCIIStop;
            peMBFrameSendCur = eMBASCIISend;
            peMBFrameReceiveCur = eMBASCIIReceive;
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
            pxMBFrameCBByteReceived = xMBASCIIReceiveFSM;
            pxMBFrameCBTransmitterEmpty = xMBASCIITransmitFSM;
            pxMBPortCBTimerExpired = xMBASCIITimerT1SExpired;

            eStatus = eMBASCIIInit( ucMBAddress, ucPort, ulBaudRate, eParity );
            break;
#endif
        default:
            eStatus = MB_EINVAL;
        }

        if( eStatus == MB_ENOERR )
        {
            if( !xMBPortEventInit(  ) )
            {
                /* port dependent event module initalization failed. */
                eStatus = MB_EPORTERR;
            }
            else
            {
                eMBCurrentMode = eMode;
                eMBRTUSlaveState = STATE_DISABLED;
            }
        }
    }
    return eStatus;
}

#if MB_TCP_ENABLED > 0
eMBErrorCode
eMBTCPInit( USHORT ucTCPPort )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( ( eStatus = eMBTCPDoInit( ucTCPPort ) ) != MB_ENOERR )
    {
        eMBRTUSlaveState = STATE_DISABLED;
    }
    else if( !xMBTCPPortEventInit(  ) )
    {
        /* Port dependent event module initalization failed. */
        eStatus = MB_EPORTERR;
    }
    else
    {
        ucMBTCPAddress = MB_TCP_PSEUDO_ADDRESS;
        eMBTCPState = STATE_DISABLED;
    }
    return eStatus;
}
#endif

eMBErrorCode
eMBRegisterCB( UCHAR ucFunctionCode, pxMBFunctionHandler pxHandler )
{
    int             i;
    eMBErrorCode    eStatus;

    if( ( 0 < ucFunctionCode ) && ( ucFunctionCode <= 127 ) )
    {
        ENTER_CRITICAL_SECTION(  );
        if( pxHandler != NULL )
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( ( xFuncHandlers[i].pxHandler == NULL ) ||
                    ( xFuncHandlers[i].pxHandler == pxHandler ) )
                {
                    xFuncHandlers[i].ucFunctionCode = ucFunctionCode;
                    xFuncHandlers[i].pxHandler = pxHandler;
                    break;
                }
            }
            eStatus = ( i != MB_FUNC_HANDLERS_MAX ) ? MB_ENOERR : MB_ENORES;
        }
        else
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {
                    xFuncHandlers[i].ucFunctionCode = 0;
                    xFuncHandlers[i].pxHandler = NULL;
                    break;
                }
            }
            /* Remove can't fail. */
            eStatus = MB_ENOERR;
        }
        EXIT_CRITICAL_SECTION(  );
    }
    else
    {
        eStatus = MB_EINVAL;
    }
    return eStatus;
}

eMBErrorCode
eMBClose( eMBMode Mode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	switch(Mode)
	{

	case MB_ASCII:
		
	case MB_RTU:
    if( eMBRTUSlaveState == STATE_DISABLED )
    {
        if( pvMBFrameCloseCur != NULL )
        {
            pvMBFrameCloseCur(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
		break;
	case MB_TCP:
		if( eMBTCPState == STATE_DISABLED )
    {
				vMBTCPPortClose(  );
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
		break;
	}
    return eStatus;
}
eMBErrorCode
eMBEnable  ( eMBMode Mode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
	switch(Mode)
	{

	case MB_ASCII:
		
	case MB_RTU:
    if( eMBRTUSlaveState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        pvMBFrameStartCur(  );
        eMBRTUSlaveState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
		break;
	case MB_TCP:
		if( eMBTCPState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        eMBTCPStart(  );
        eMBTCPState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
		break;
	}
    return eStatus;
}

eMBErrorCode
eMBDisable( eMBMode Mode )
{
    eMBErrorCode    eStatus= MB_ENOERR;
	switch(Mode)
	{

	case MB_ASCII:
		
	case MB_RTU:
    if( eMBRTUSlaveState == STATE_ENABLED )
    {
        pvMBFrameStopCur(  );
        eMBRTUSlaveState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBRTUSlaveState == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
		break;
	case MB_TCP:
		if( eMBTCPState == STATE_ENABLED )
    {
        eMBTCPStop(  );
        eMBTCPState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBTCPState == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
		break;
	}
    return eStatus;
}



LastCommandTpye last10Command[10];
int currentIndex = 0;
extern osMessageQueueId_t MBcommand_queueHandle;
void saveCommand(const UCHAR* command,USHORT   usTCPLength) {
    // ���������ʷ��¼������
		const uint8_t hex[] = "0123456789ABCDEF";
    strncpy(last10Command[currentIndex].command, command, MAX_COMMAND_LENGTH);
		last10Command[currentIndex].length=usTCPLength;

		sdk_addLog(Command,last10Command[currentIndex].command,usTCPLength);
    currentIndex = (currentIndex + 1) % MAX_HISTORY_COMMANDS;
}

eMBErrorCode
eMBPoll( void )
{

    static USHORT   usLength;
	
    static UCHAR   *ucMBTCPFrame;
    static UCHAR    ucTCPRcvAddress;
    static UCHAR    ucTCPFunctionCode;
    static USHORT   usTCPLength;	
    static eMBException eException;

    int             i;
    eMBErrorCode    eStatus = MB_ENOERR;
    eMBEventType    eEvent;

    /* Check if the protocol stack is ready. */
    if( eMBRTUSlaveState != STATE_ENABLED && eMBTCPState != STATE_ENABLED )
    {
        return MB_EILLSTATE;
    }
		/*TCP event*/
		if( xMBTCPPortEventGet( &eEvent ) == TRUE )
    {
        switch ( eEvent )
        {
        case EV_READY:
            break;

        case EV_FRAME_RECEIVED:
            eStatus = eMBTCPReceive( &ucTCPRcvAddress, &ucMBTCPFrame, &usTCPLength );
            if( eStatus == MB_ENOERR )
            {		
                /* Check if the frame is for us. If not ignore the frame. */
                if( ( ucTCPRcvAddress == ucMBTCPAddress ) || ( ucTCPRcvAddress == MB_ADDRESS_BROADCAST ) )
                {		
                    ( void )xMBTCPPortEventPost( EV_EXECUTE );
                }
            }
            break;

        case EV_EXECUTE:
            ucTCPFunctionCode = ucMBTCPFrame[MB_PDU_FUNC_OFF];
						if(ucTCPFunctionCode==0x09 || ucTCPFunctionCode==0x0a || ucTCPFunctionCode==0x0b|| ucTCPFunctionCode==0x0c|| ucTCPFunctionCode==0x0d)
						{saveCommand(ucMBTCPFrame,usTCPLength);}
            eException = MB_EX_ILLEGAL_FUNCTION;
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                /* No more function handlers registered. Abort. */
                if( xFuncHandlers[i].ucFunctionCode == 0 )
                {
                    break;
                }
                else if( xFuncHandlers[i].ucFunctionCode == ucTCPFunctionCode )
                {
                    eException = xFuncHandlers[i].pxHandler( ucMBTCPFrame, &usTCPLength );
                    break;
                }
            }

            /* If the request was not sent to the broadcast address we
             * return a reply. */
            if( ucTCPRcvAddress != MB_ADDRESS_BROADCAST )
            {
                if( eException != MB_EX_NONE )
                {
                    /* An exception occured. Build an error frame. */
                    usTCPLength = 0;
                    ucMBTCPFrame[usLength++] = ( UCHAR )( ucTCPFunctionCode | MB_FUNC_ERROR );
                    ucMBTCPFrame[usLength++] = eException;
                }                
                eStatus = eMBTCPSend( ucMBTCPAddress, ucMBTCPFrame, usTCPLength );
            }
            break;

        case EV_FRAME_SENT:
            break;
        }
    }
    return MB_ENOERR;
}
