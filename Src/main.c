
#include "main.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "dac.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "systemClock.h"
#include "modbusToAndroid.h"
#include "dataProcessing.h"

int main(void)
{

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_DAC_Init();
	MX_ADC1_Init();
	MX_USART1_UART_Init();
	//MX_USART2_UART_Init();
	MX_TIM6_Init();
//	MX_TIM7_Init();
	MX_I2C1_Init();

	uint16_t sendMasterCount;

	while (1)
	{
		if (tim6Flag)
		{
			tim6Flag = 0;
			UsartRxMonitor();//接收处理数据
			sendMasterCount++;
			if (sendMasterCount==100) sendDataMaster03();
			if (sendMasterCount==200) sendDataMaster16();
			if (sendMasterCount > 200) {
				sendMasterCount = 0;
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_2);
				dataProcessing();//数据处理
			}
		}
	}
}
