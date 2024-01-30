#include "sensorAdapter_sdk.h"

/* ��ʪ����ֵ */

/* �����ʮ������ */
extern uint8_t last10dataIndex;
extern uint32_t last10data[10];
/* �����ݷ���modbusTCP�Ĵ����� ��һ������Ϊ���ݸ���������Ϊ���� */
extern uint16_t usRegInputBuf[REG_INPUT_NREGS];
void sdk_addDataToReg(uint8_t arg_cnt,...){
    va_list p_args;
    va_start(p_args, arg_cnt);
    int idx;
    uint16_t val;
    
    for(idx = 1; idx <= arg_cnt; ++idx){
        val = va_arg(p_args, int);
			usRegInputBuf[idx+6]=val;
			/* �����ݷ���Ĵ���ͬʱ�������ʮ������ */
			last10data[last10dataIndex]=val;
      last10dataIndex=(last10dataIndex+1)%10;
    }
		va_end(p_args);
		osDelay(10);
}



/* ����IWDG ��һ������Ϊʱ�� ��λs */
void sdk_startIWDG(uint8_t seconds){
	MX_IWDG_Init(IWDG_PRESCALER_64,500*seconds);
}


/* ��¼��־ ��һ������Ϊ��־����	�ڶ�������Ϊֵ */
/* command�ļ�¼�Ѿ���� ÿ��command���ᱻ��¼ ���������log��¼ֻ������ݺʹ����д�� */
extern RTC_TimeTypeDef GetTime;
extern RTC_DateTypeDef GetDate;
LogCacheTypeDef logCache[MAX_LOG_ENTRIES];
uint16_t logCallbackCount=0;
extern osSemaphoreId_t LogSemaphoreHandle;
void sdk_addLog(Log_TypeDef logtpye,const char * buff,uint16_t value){
  if (logCallbackCount < MAX_LOG_ENTRIES ) {
		osSemaphoreAcquire(LogSemaphoreHandle,osWaitForever);
    // ����־��ӵ�����
		/* ��¼�˿�ʱ�� */
		HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &GetDate, RTC_FORMAT_BIN);	
		logCache[logCallbackCount].year=GetDate.Year;
		logCache[logCallbackCount].month=GetDate.Month;
		logCache[logCallbackCount].date=GetDate.Date;
		logCache[logCallbackCount].hour=GetTime.Hours;
		logCache[logCallbackCount].minute=GetTime.Minutes;
		logCache[logCallbackCount].second=GetTime.Seconds+(255.0-GetTime.SubSeconds)/256;
		
		logCache[logCallbackCount].logtype = logtpye;
		logCache[logCallbackCount].value = value;
    strncpy(logCache[logCallbackCount].info, buff, MAX_LOG_LENGTH - 1);
    logCache[logCallbackCount].info[MAX_LOG_LENGTH - 1] = '\0'; // ȷ���ַ����� null ��β
    logCallbackCount++;
		osSemaphoreRelease(LogSemaphoreHandle);
    }
}
/* RS485��ʼ�� */
void sdk_rs485Init(	uint32_t baudrate,uint8_t WordLength,uint8_t stopbit,uint8_t Parity){
 MX_USART2_UART_Init(	baudrate,WordLength,stopbit, Parity);
}


/* RS232��ʼ�� */
void sdk_rs232Init(	uint32_t baudrate,uint8_t WordLength,uint8_t stopbit,uint8_t Parity){
	MX_USART1_UART_Init(	baudrate,WordLength,stopbit, Parity);
}

/* ϵͳ���������ʼ�� */
void sdk_sysInit(void){
	HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	I2C_EE_Init();
	RTC_CLK_Config();

}
/* OS��ʼ�� ������һ��������Ϊ���ݴ�������*/
void sdk_startMain(void (*osThreadFunc_t) (void *argument)){
	osKernelInitialize();
  MX_FREERTOS_Init(osThreadFunc_t);
  osKernelStart();
}

/* ����rs485���� ��һ������Ϊ����ָ�� �ڶ�������Ϊ�����С*/
uint8_t recvFlag;
void sdk_rs485Recv(uint8_t *array,uint8_t arraySize){

	HAL_UART_Receive_IT(&huart2,array,arraySize);
	recvFlag = arraySize;

}

/* ����rs232���� ��һ������Ϊ����ָ�� �ڶ�������Ϊ�����С*/
void sdk_rs232Recv(uint8_t *array,uint8_t arraySize){

	HAL_UART_Receive_IT(&huart1,array,arraySize);

}

/* �ȴ�rs485���� */
uint8_t sdk_rs485Wait(uint16_t ticks){
	TickType_t xTicksToWait = ticks / portTICK_PERIOD_MS;
	if(0x01 == osThreadFlagsWait(0x01,osFlagsWaitAll,xTicksToWait))
	{
		
		return 0x01;
	}
	return 0x00;
}

/* �ȴ�rs232���� */
uint8_t sdk_rs232Wait(uint8_t ticks){

	if(0x01 == osThreadFlagsWait(0x01,osFlagsWaitAll,ticks))
	{
		return 0x01;
	}
	return 0x00;
}

/* rs485���ͺ���*/
void	sdk_rs485Send(uint8_t *data){
	HAL_UART_Transmit(&huart2,data,sizeof(data) / sizeof(data[0]),HAL_MAX_DELAY);
}

/* rs232���ͺ���*/
void	sdk_rs232Send(uint8_t *data){
	HAL_UART_Transmit(&huart1,data,sizeof(data) / sizeof(data[0]),HAL_MAX_DELAY);
}
/* SDK��ʱ���� */
void sdk_delayMs(uint16_t ticks){
	osDelay(ticks);
}

/* ��ʼ��ADC */
void sdk_adcInit(void){
		MX_ADC1_Init();
}

/* ��ȡadc��ֵ */
uint32_t sdk_getadcValue(void){
		HAL_ADC_Start(&hadc1); 
		return HAL_ADC_GetValue(&hadc1);
}

