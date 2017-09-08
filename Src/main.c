
#include "main.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "dac.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "dma.h"
#include "systemClock.h"
#include "modbusToAndroid.h"
#include "dataProcessing.h"

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_DAC_Init();
	MX_ADC1_Init();
	MX_USART1_UART_Init();
	MX_TIM6_Init();

	uint16_t sendMasterCount;

	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, ADC_ConvertedValue, 7);

	while (1)
	{
		UsartRxMonitor();
		dataProcessing();

		if (tim6Flag)
		{
			tim6Flag = 0;
			sendMasterCount++;
			if (sendMasterCount==100) sendDataMaster03();
			if (sendMasterCount==200) sendDataMaster16();
			if (sendMasterCount > 200) {
				sendMasterCount = 0;
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_2);
			}
		}
	}
}


