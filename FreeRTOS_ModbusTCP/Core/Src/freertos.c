/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mb.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "usart.h"
#include "bsp_sdio_sd.h"
#include "IWDG.h"
#include "event_groups.h"
#include "spi.h"
#include "sht30.h"
#include "MPU6050.h"
#include "at24c02.h"
#include "md0130.h"
#include <string.h>
#include "adc.h"
#include "mb.h"
#include "logo.h"
#include "sensorAdapter_sdk.h"
/* USER CODE END Includes */
extern RTC_HandleTypeDef hrtc;
uint8_t  temp_Threshold;		
uint8_t  hum_Threshold;
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
	uint8_t configrationInfo[32];

osThreadId_t ModbusTCP_taskHandle;
osThreadId_t IWDG_taskHandle;
osThreadId_t SHT30_taskHandle;
osThreadId_t MPU6050_taskHandle;
osThreadId_t LCD_taskHandle;
osThreadId_t ADC_taskHandle;
osThreadId_t reboot_taskHandle;
osThreadId_t userDef_taskHandle;

const osThreadAttr_t ModbusTCP_task_attributes = {
  .name = "ModbusTCP_task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh6,
};
const osThreadAttr_t IWDG_task_attributes = {
  .name = "IWDG_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
const osThreadAttr_t SHT30_task_attributes = {
  .name = "SHT30_task",
  .stack_size = 200 * 4,
  .priority = (osPriority_t) osPriorityHigh2,
};
const osThreadAttr_t MPU6050_task_attributes = {
  .name = "MPU6050_task",
  .stack_size = 150 * 4,
  .priority = (osPriority_t) osPriorityHigh2,
};
const osThreadAttr_t LCD_task_attributes = {
  .name = "LCD_task",
  .stack_size = 200 * 4,
  .priority = (osPriority_t) osPriorityHigh2,
};

const osThreadAttr_t ADC_task_attributes = {
  .name = "ADC_task",
  .stack_size = 120 * 4,
  .priority = (osPriority_t) osPriorityHigh2,
};

const osThreadAttr_t reboot_task_attributes = {
  .name = "reboot_task",
  .stack_size = 200,
  .priority = (osPriority_t) osPriorityHigh6,
};
const osThreadAttr_t userDef_task_attributes = {
  .name = "userDef_task",
  .stack_size = 2000,
  .priority = (osPriority_t) osPriorityHigh6,
};
/* USER CODE END FunctionPrototypes */
osTimerId_t myTimer01Handle;
const osTimerAttr_t myTimer01_attributes = {
  .name = "myTimer01"
};
void ModbusTCP_main(void *argument);
void IWDG_main(void *argument);
void SHT30_main(void *argument);
void MPU6050_main(void *argument);
void LCD_main(void *argument);
void ADC_main(void *argument);
void reboot_main(void *argument);

extern void MX_LWIP_Init(uint8_t ip1,uint8_t ip2,uint8_t ip3,uint8_t ip4);


osEventFlagsId_t iwdgEventHandle;
const osEventFlagsAttr_t iwdgEvent_attributes = {
  .name = "iwdgEvent"
};


LastErrorTpye last10errors[10];
uint8_t errorIndex=0;
void addError(const char * buff,uint8_t value){	
	if(value!=0){
		sprintf(last10errors[errorIndex].errorMessage,"%s :%d",buff,value);
		errorIndex=(errorIndex+1)%10;}
	else{
		strcpy(last10errors[errorIndex].errorMessage,buff);
		errorIndex=(errorIndex+1)%10;
	}
}

osSemaphoreId_t LogSemaphoreHandle;
const osSemaphoreAttr_t LogSemaphore_attributes = {
  .name = "LogSemaphore"
};

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void (*osThreadFunc_t) (void *argument)) {
  /* USER CODE BEGIN Init */
//	uint8_t writeBuf[32]={0xff,0xff,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,    0xc0,0xa8,0x01,0x20,0x01,0xf6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//	I2C_EE_BufferWrite(writeBuf,0x00,32);
	/* 上电之后初始化温度湿度阈值 数值存放在eeprom中 */
	I2C_EE_BufferRead(configrationInfo,0x00,32);
	temp_Threshold = configrationInfo[0];
	hum_Threshold  = configrationInfo[1];

	print_usart2("####reboot###");
	sdk_addLog(Info,"reboot",0);

  myTimer01Handle = osTimerNew(PeriodicLogCallback, osTimerPeriodic, NULL, &myTimer01_attributes);
  LogSemaphoreHandle = osSemaphoreNew(1, 1, &LogSemaphore_attributes);

  /* creation of ModbusTCP_task */
  ModbusTCP_taskHandle = osThreadNew(ModbusTCP_main, NULL, &ModbusTCP_task_attributes);
	
	userDef_taskHandle = osThreadNew(osThreadFunc_t,NULL,&userDef_task_attributes);
	
	/* creation of IWDG_task */
  IWDG_taskHandle = osThreadNew(IWDG_main, NULL, &IWDG_task_attributes);


	/* creation of SHT30_task */
	SHT30_taskHandle = osThreadNew(SHT30_main, NULL, &SHT30_task_attributes);


	/* creation of MPU6050_task */
  MPU6050_taskHandle = osThreadNew(MPU6050_main, NULL, &MPU6050_task_attributes);

	/* creation of LCD_task */
  LCD_taskHandle = osThreadNew(LCD_main, NULL, &LCD_task_attributes);
	
	/* creation of ADC_task */
  ADC_taskHandle = osThreadNew(ADC_main, NULL, &ADC_task_attributes);

	/* creation of reboot_task */
  reboot_taskHandle = osThreadNew(reboot_main, NULL, &reboot_task_attributes);

	iwdgEventHandle = osEventFlagsNew(&iwdgEvent_attributes);
  /* USER CODE END RTOS_EVENTS */
	osTimerStart(myTimer01Handle,1*60*1000);
	
}
void reboot_main(void *argument){
	for(;;){
		osEventFlagsSet(iwdgEventHandle,TASK_BIT_4 );
		osDelay(1000);
	}
}

RTC_TimeTypeDef GetTime;
RTC_DateTypeDef GetDate;
/* USER CODE BEGIN Header_ModbusTCP_main */
/**
* @brief Function implementing the ModbusTCP_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ModbusTCP_main */
void ModbusTCP_main(void *argument)
{
  /* USER CODE BEGIN ModbusTCP_main */
	if(configrationInfo[16]==0 && configrationInfo[17]==0&&configrationInfo[18]==0&&configrationInfo[19]==0){
		configrationInfo[16]=192;
		configrationInfo[17]=168;
		configrationInfo[18]=1;
		configrationInfo[19]=20;
		configrationInfo[20]=0x01;
		configrationInfo[21]=0xf6;
	}
	/* init code for LWIP */
  MX_LWIP_Init(configrationInfo[16],configrationInfo[17],configrationInfo[18],configrationInfo[19]);
	eMBTCPInit(configrationInfo[20]<<8|configrationInfo[21] );
	eMBEnable(MB_TCP);
	osDelay(100);
  /* Infinite loop */
  for(;;)
  {
		eMBPoll();
		osEventFlagsSet(iwdgEventHandle,TASK_BIT_1 );
		HAL_GPIO_TogglePin(led1_GPIO_Port,led1_Pin);
		osDelay(100);
  }
  /* USER CODE END ModbusTCP_main */
}



/*IWDG main function*/
extern IWDG_HandleTypeDef hiwdg;
void IWDG_main(void *argument)
{		 
	uint32_t uxBits;
	for(;;)
  {	
		/*wait for all bits are set to 1*/
//		uxBits = osEventFlagsWait(iwdgEventHandle,TASK_BIT_ALL,0x03,1000);
		uxBits = osEventFlagsGet(iwdgEventHandle);
		/*if all bits are set to 1 feed dog*/
		if((uxBits & TASK_BIT_ALL) == TASK_BIT_ALL)
		{			
			HAL_IWDG_Refresh(&hiwdg);
			xEventGroupClearBits(iwdgEventHandle,TASK_BIT_ALL);
		}else{
			print_usart2("###### iwdg %x #####\r\n",uxBits);
		}
		osDelay(1000);
	}
}


/*DHT11 main function*/
extern uint16_t usRegInputBuf[REG_INPUT_NREGS];
void SHT30_main(void *argument){
	osDelay(500);

	for(;;)
  {	
		Sht30_Init();
		osDelay(100);
		float temp,hum;
		Read_IIC_Data(&temp,&hum);
		usRegInputBuf[4]=temp;
		usRegInputBuf[5]=hum;
		osDelay(900);
		/* 超过温度湿度阈值 触发log记录 */
		if(usRegInputBuf[4]>temp_Threshold ){
			sdk_addLog(Error,"overtemperature",usRegInputBuf[4]);
			addError("[ERROR]overtemperature",usRegInputBuf[4]);
		}
		if(usRegInputBuf[5]>hum_Threshold){
			addError("[ERROR]overhumidity",usRegInputBuf[5]);
			sdk_addLog(Error,"overhumidity",usRegInputBuf[5]);
		}
	}

}


/*MPU6050 main function*/
short gx,gy,gz;
void MPU6050_main(void *argument){
	while(MPU6050_Init()){
		addError("[ERROR]MPU6050_Init Failed",0);
		osThreadSuspend(MPU6050_taskHandle);
	}	
	for(;;)
	{				
		HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &GetDate, RTC_FORMAT_BIN);	
		MPU_Get_Gyroscope(&gx,&gy,&gz);
		usRegInputBuf[0]=gx;
		usRegInputBuf[1]=gy;
		usRegInputBuf[2]=gz;
		osDelay(1000);
	}
}



/*LCD main function*/
extern LastCommandTpye last10Command[10];
const uint8_t hex[] = "0123456789ABCDEF";

uint32_t hexDisplay(uint8_t sou){
	uint32_t res;
	res=10*(sou>>4)+(sou&0x0f);
	return res;
}
uint8_t last10dataIndex;
uint32_t last10data[10];
extern uint8_t lcdDisplayStat;
extern uint8_t IP_ADDRESS[4];
extern int currentIndex;
char LCDbuff[12];
void LCD_main(void *argument){
	atk_md0130_init();
  atk_md0130_clear(ATK_MD0130_WHITE);	
	for(;;)
	{		
		atk_md0130_display_on();	
		atk_md0130_clear(ATK_MD0130_WHITE);	
		switch(lcdDisplayStat){
			case 0:
					atk_md0130_show_pic(0,0,240,220,(uint8_t *)gImage_logo);

					atk_md0130_show_string(10, 115,"IP:", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_xnum(60, 115,IP_ADDRESS[0],3,ATK_MD0130_NUM_SHOW_ZERO,ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_string(95, 115,".", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_xnum(100, 115,IP_ADDRESS[1],3,ATK_MD0130_NUM_SHOW_ZERO,ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_string(135, 115,".", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_xnum(140, 115,IP_ADDRESS[2],3,ATK_MD0130_NUM_SHOW_ZERO,ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_string(175, 115,".", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_xnum(180, 115,IP_ADDRESS[3],3,ATK_MD0130_NUM_SHOW_ZERO,ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_string(10, 155, "Port:", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_xnum(75, 155,configrationInfo[20]<<8|configrationInfo[21] ,3,ATK_MD0130_NUM_SHOW_ZERO,ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_string(10, 190, "Version:", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_xnum(100, 190,configrationInfo[2],3,ATK_MD0130_NUM_SHOW_NOZERO,ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_string(140, 190, ".", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_xnum(142, 190,configrationInfo[3],2,ATK_MD0130_NUM_SHOW_NOZERO,ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
			break;
			case 1:
					atk_md0130_show_pic(0,0,240,220,(uint8_t *)gImage_logo);
					atk_md0130_show_string(10, 115,"Free Stack:", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_xnum(140, 115,xPortGetFreeHeapSize(),5,ATK_MD0130_NUM_SHOW_NOZERO,ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_string(10, 155,"System usage:", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_xnum(140, 155,100*(float)(configTOTAL_HEAP_SIZE-xPortGetFreeHeapSize())/configTOTAL_HEAP_SIZE,5,ATK_MD0130_NUM_SHOW_NOZERO,ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
					atk_md0130_show_string(200, 155,"%", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLACK);
			break;
			case 2:
					atk_md0130_show_string(10, 10, "Last 10 commands", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLUE);
			for(int j=0;j<10;j++){
				for(int i=0;i<last10Command[(currentIndex+j)% MAX_HISTORY_COMMANDS].length;i++){
					atk_md0130_show_char(10+(16*i),45+18*j,hex[((last10Command[(currentIndex+j)% MAX_HISTORY_COMMANDS].command[i])>>4)], ATK_MD0130_LCD_FONT_16, ATK_MD0130_RED);
					atk_md0130_show_char(18+(16*i),45+18*j,hex[((last10Command[(currentIndex+j)% MAX_HISTORY_COMMANDS].command[i])&0x0f)], ATK_MD0130_LCD_FONT_16, ATK_MD0130_RED);
				}}
			break;
			case 3:
					atk_md0130_show_string(10, 10, "Last 10 data", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLUE);
					for(int j=0;j<10;j++){
						for(int i=0;i<8;i++){
						atk_md0130_show_char(10+10*i,45+18*j,hex[(last10data[(last10dataIndex+j)% MAX_HISTORY_COMMANDS]>>(4*(7-i))) &0xf ], ATK_MD0130_LCD_FONT_16, ATK_MD0130_RED);
					}
				}
			break;			
			case 4:
					atk_md0130_show_string(10, 10, "Last 10 errors", ATK_MD0130_LCD_FONT_24, ATK_MD0130_BLUE);
			for(int i=0;i<10;i++){
					atk_md0130_show_string(10, 45+18*i,last10errors[(errorIndex+i)%10].errorMessage , ATK_MD0130_LCD_FONT_16, ATK_MD0130_RED);
			}
			break;
		}
		osDelay(500);
		osThreadSuspend(LCD_taskHandle);
	}
}




/*ADC main function*/
/* 6存放低8位 7存放高8位*/
void ADC_main(void *argument){
	MX_ADC1_Init();
	for(;;){
		HAL_ADC_Start(&hadc1); 
		usRegInputBuf[6] = HAL_ADC_GetValue(&hadc1);
		osDelay(500);
	}
}

/*log task main function*/
FATFS fs;
FIL fnew;
char SDPath[4];
FRESULT res_sd;
extern LogCacheTypeDef logCache[MAX_LOG_ENTRIES];
char timeStampBuf[24];
char commandBuf[20];
UINT fnum;
extern uint16_t logCallbackCount;
char logFileName[14];
void PeriodicLogCallback(void *argument){
				if((!HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_3))&&logCallbackCount!=0){
					/* 获取二值信号量 防止多线程对日志缓存的操作 */
					osSemaphoreAcquire(LogSemaphoreHandle,osWaitForever);
					FATFS_LinkDriver(&SD_Driver, SDPath);
					res_sd = f_mount(&fs,"0:",1);
					if(res_sd!=FR_OK){
						f_mount(NULL,"0:",1);
					}else
					{
					}
					HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
					HAL_RTC_GetDate(&hrtc, &GetDate, RTC_FORMAT_BIN);
					memset(logFileName,0,sizeof(logFileName));
					sprintf(logFileName,"%02d%02d%02d.txt",2000 + GetDate.Year, GetDate.Month, GetDate.Date);
					res_sd = f_open( &fnew,logFileName,FA_OPEN_ALWAYS | FA_WRITE );
					/* 开始逐条写cache */
					if ( res_sd == FR_OK )
					{
						HAL_IWDG_Refresh(&hiwdg);
						/* 移动指针到文件末尾 */
						f_lseek(&fnew,fnew.fsize);//move the pointer to last word of the file
						for(int i=0;i<logCallbackCount;i++){
							switch(logCache[i].logtype){
								case Command:
									res_sd=f_write(&fnew,"[CMD]\t",sizeof("[CMD]\t"),&fnum);
									memset(timeStampBuf,0,sizeof(timeStampBuf));
									sprintf(timeStampBuf,"%02d-%02d-%02d %02d:%02d:%06.3f",2000 + logCache[i].year, logCache[i].month, logCache[i].date,logCache[i].hour, logCache[i].minute, logCache[i].second);
									res_sd=f_write(&fnew,timeStampBuf,sizeof(timeStampBuf),&fnum);
								
									char tempBuf[40];
									for(int j=0;j<logCache[i].value;j++){
										tempBuf[2*j]=hex[(logCache[i].info[j])>>4];
										tempBuf[2*j+1]=hex[(logCache[i].info[j])&0x0f];
									}
									res_sd=f_write(&fnew,tempBuf,2*(logCache[i].value),&fnum);
									res_sd=f_write(&fnew,"\r\n",sizeof("\r\n"),&fnum);
									break;
								case Info:
									res_sd=f_write(&fnew,"[INFO]\t",sizeof("[INFO]\t"),&fnum);
									memset(timeStampBuf,0,sizeof(timeStampBuf));
									sprintf(timeStampBuf,"%02d-%02d-%02d %02d:%02d:%06.3f",2000 + logCache[i].year, logCache[i].month, logCache[i].date,logCache[i].hour, logCache[i].minute, logCache[i].second);
									res_sd=f_write(&fnew,timeStampBuf,sizeof(timeStampBuf),&fnum);	
									memset(commandBuf,0,sizeof(commandBuf));
									sprintf(commandBuf,"%s:%x",logCache[i].info,logCache[i].value);
									res_sd=f_write(&fnew,commandBuf,sizeof(commandBuf),&fnum);
									res_sd=f_write(&fnew,"\r\n",sizeof("\r\n"),&fnum);
									break;
								case Error:
									res_sd=f_write(&fnew,"[ERR]\t",sizeof("[ERR]\t"),&fnum);
									memset(timeStampBuf,0,sizeof(timeStampBuf));
									sprintf(timeStampBuf,"%02d-%02d-%02d %02d:%02d:%06.3f",2000 + logCache[i].year, logCache[i].month, logCache[i].date,logCache[i].hour, logCache[i].minute, logCache[i].second);
									res_sd=f_write(&fnew,timeStampBuf,sizeof(timeStampBuf),&fnum);	
									memset(commandBuf,0,sizeof(commandBuf));
									sprintf(commandBuf,"%s:%x",logCache[i].info,logCache[i].value);
									res_sd=f_write(&fnew,commandBuf,sizeof(commandBuf),&fnum);
									res_sd=f_write(&fnew,"\r\n",sizeof("\r\n"),&fnum);
									break;
								case Data:
									res_sd=f_write(&fnew,"[DATA]\t",sizeof("[DATA]\t"),&fnum);
									memset(timeStampBuf,0,sizeof(timeStampBuf));
									sprintf(timeStampBuf,"%02d-%02d-%02d %02d:%02d:%06.3f",2000 + logCache[i].year, logCache[i].month, logCache[i].date,logCache[i].hour, logCache[i].minute, logCache[i].second);
									res_sd=f_write(&fnew,timeStampBuf,sizeof(timeStampBuf),&fnum);	
									memset(commandBuf,0,sizeof(commandBuf));
									sprintf(commandBuf,"%s:%x",logCache[i].info,logCache[i].value);
									res_sd=f_write(&fnew,commandBuf,sizeof(commandBuf),&fnum);
									res_sd=f_write(&fnew,"\r\n",sizeof("\r\n"),&fnum);
									break;						
							}
							f_sync(&fnew);
						}
						memset(logCache,0,sizeof(logCache));
						logCallbackCount=0;
						HAL_GPIO_TogglePin(led2_GPIO_Port,led2_Pin);
					}
					f_close(&fnew);
					/* 释放二值信号量 允许其他线程修改日志缓存 */
					osSemaphoreRelease(LogSemaphoreHandle);
			}
}

