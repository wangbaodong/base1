#ifndef __SENSORADPTER_SDK_H_
#define __SENSORADPTER_SDK_H_

#include "main.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "usart.h"
#include "gpio.h"
#include "tim.h"
#include "rtc.h"
#include "stdio.h"
#include <stdarg.h>
#include "at24c02.h"
#include "md0130.h"
#include <math.h>
#include "sht30.h"
#include "adc.h"
#include "iwdg.h"
#include "sdio.h"
#include "fatfs.h"
/* 日志信息中的可填写的字符串的长度 */
#define MAX_COMMAND_LENGTH 20
#define MAX_HISTORY_COMMANDS 10
/* 日志缓存的最大数量 */
#define MAX_LOG_ENTRIES 500
#define MAX_LOG_LENGTH 20
/* 日志类型 */
typedef enum 
{
  Command       = 0x01,
  Data   				= 0x02,
  Error    			= 0x03,
	Info					= 0x04,
} Log_TypeDef;

typedef struct {
	Log_TypeDef logtype;
	char info[MAX_LOG_LENGTH];
	uint16_t value;
	/* 记录日志时的时间戳 */
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	double second;
	
} LogCacheTypeDef;

void sdk_addDataToReg(uint8_t arg_cnt,...);
void sdk_startIWDG(uint8_t seconds);
void sdk_addLog(Log_TypeDef logtpye,const char * buff,uint16_t value);
void sdk_rs485Init(	uint32_t baudrate,uint8_t WordLength,uint8_t stopbit,uint8_t Parity);
void sdk_rs232Init(	uint32_t baudrate,uint8_t WordLength,uint8_t stopbit,uint8_t Parity);
void sdk_sysInit(void);
void sdk_startMain(void (*osThreadFunc_t) (void *argument));
void sdk_rs485Recv(uint8_t *array,uint8_t arraySize);
void sdk_rs232Recv(uint8_t *array,uint8_t arraySize);
void sdk_rs485Send(uint8_t *data);
void sdk_rs232Send(uint8_t *data);
void sdk_delayMs(uint16_t ticks);
void sdk_adcInit(void);
uint8_t sdk_rs485Wait(uint16_t ticks);
uint8_t sdk_rs232Wait(uint8_t ticks);
uint32_t sdk_getadcValue(void);



#endif

