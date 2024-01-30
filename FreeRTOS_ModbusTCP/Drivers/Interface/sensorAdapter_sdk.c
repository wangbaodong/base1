#include "sensorAdapter_sdk.h"

/* 温湿度阈值 */

/* 最近的十条数据 */
extern uint8_t last10dataIndex;
extern uint32_t last10data[10];
/* 将数据放入modbusTCP寄存器中 第一个参数为数据个数，接着为数据 */
extern uint16_t usRegInputBuf[REG_INPUT_NREGS];
void sdk_addDataToReg(uint8_t arg_cnt,...){
    va_list p_args;
    va_start(p_args, arg_cnt);
    int idx;
    uint16_t val;
    
    for(idx = 1; idx <= arg_cnt; ++idx){
        val = va_arg(p_args, int);
			usRegInputBuf[idx+6]=val;
			/* 将数据放入寄存器同时更新最近十条数据 */
			last10data[last10dataIndex]=val;
      last10dataIndex=(last10dataIndex+1)%10;
    }
		va_end(p_args);
		osDelay(10);
}



/* 配置IWDG 第一个参数为时间 单位s */
void sdk_startIWDG(uint8_t seconds){
	MX_IWDG_Init(IWDG_PRESCALER_64,500*seconds);
}


/* 记录日志 第一个参数为日志类型	第二个参数为值 */
/* command的记录已经完成 每个command都会被记录 所以这里的log记录只针对数据和错误的写入 */
extern RTC_TimeTypeDef GetTime;
extern RTC_DateTypeDef GetDate;
LogCacheTypeDef logCache[MAX_LOG_ENTRIES];
uint16_t logCallbackCount=0;
extern osSemaphoreId_t LogSemaphoreHandle;
void sdk_addLog(Log_TypeDef logtpye,const char * buff,uint16_t value){
  if (logCallbackCount < MAX_LOG_ENTRIES ) {
		osSemaphoreAcquire(LogSemaphoreHandle,osWaitForever);
    // 将日志添加到缓存
		/* 记录此刻时间 */
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
    logCache[logCallbackCount].info[MAX_LOG_LENGTH - 1] = '\0'; // 确保字符串以 null 结尾
    logCallbackCount++;
		osSemaphoreRelease(LogSemaphoreHandle);
    }
}
/* RS485初始化 */
void sdk_rs485Init(	uint32_t baudrate,uint8_t WordLength,uint8_t stopbit,uint8_t Parity){
 MX_USART2_UART_Init(	baudrate,WordLength,stopbit, Parity);
}


/* RS232初始化 */
void sdk_rs232Init(	uint32_t baudrate,uint8_t WordLength,uint8_t stopbit,uint8_t Parity){
	MX_USART1_UART_Init(	baudrate,WordLength,stopbit, Parity);
}

/* 系统基本组件初始化 */
void sdk_sysInit(void){
	HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	I2C_EE_Init();
	RTC_CLK_Config();

}
/* OS初始化 传入另一个函数作为数据处理任务*/
void sdk_startMain(void (*osThreadFunc_t) (void *argument)){
	osKernelInitialize();
  MX_FREERTOS_Init(osThreadFunc_t);
  osKernelStart();
}

/* 配置rs485接收 第一个参数为数组指针 第二个参数为数组大小*/
uint8_t recvFlag;
void sdk_rs485Recv(uint8_t *array,uint8_t arraySize){

	HAL_UART_Receive_IT(&huart2,array,arraySize);
	recvFlag = arraySize;

}

/* 配置rs232接收 第一个参数为数组指针 第二个参数为数组大小*/
void sdk_rs232Recv(uint8_t *array,uint8_t arraySize){

	HAL_UART_Receive_IT(&huart1,array,arraySize);

}

/* 等待rs485接收 */
uint8_t sdk_rs485Wait(uint16_t ticks){
	TickType_t xTicksToWait = ticks / portTICK_PERIOD_MS;
	if(0x01 == osThreadFlagsWait(0x01,osFlagsWaitAll,xTicksToWait))
	{
		
		return 0x01;
	}
	return 0x00;
}

/* 等待rs232接收 */
uint8_t sdk_rs232Wait(uint8_t ticks){

	if(0x01 == osThreadFlagsWait(0x01,osFlagsWaitAll,ticks))
	{
		return 0x01;
	}
	return 0x00;
}

/* rs485发送函数*/
void	sdk_rs485Send(uint8_t *data){
	HAL_UART_Transmit(&huart2,data,sizeof(data) / sizeof(data[0]),HAL_MAX_DELAY);
}

/* rs232发送函数*/
void	sdk_rs232Send(uint8_t *data){
	HAL_UART_Transmit(&huart1,data,sizeof(data) / sizeof(data[0]),HAL_MAX_DELAY);
}
/* SDK延时函数 */
void sdk_delayMs(uint16_t ticks){
	osDelay(ticks);
}

/* 初始化ADC */
void sdk_adcInit(void){
		MX_ADC1_Init();
}

/* 获取adc数值 */
uint32_t sdk_getadcValue(void){
		HAL_ADC_Start(&hadc1); 
		return HAL_ADC_GetValue(&hadc1);
}

