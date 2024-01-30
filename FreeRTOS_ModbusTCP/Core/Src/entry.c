#include "sensorAdapter_sdk.h"
#include "entry.h"


int main(){

	sdk_sysInit();
	sdk_startIWDG(4);
	sdk_rs485Init(115200,UART_WORDLENGTH_8B,UART_STOPBITS_1,UART_PARITY_NONE);
	sdk_startMain(userDef_main);

}

 
void userDef_main(void *argument){

	for(;;){
		sdk_addLog(Error,"er",66);
		sdk_addLog(Data,"dat",66);
		sdk_addDataToReg(1,0x22);
		sdk_delayMs(2000);
	}
}

void sdk_MBCallBack(int value){
	if(value == 0x01){
		HAL_GPIO_TogglePin(led3_GPIO_Port,led3_Pin);
	}

}

