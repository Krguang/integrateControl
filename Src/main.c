
#include "main.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "dac.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "modbusToAndroid.h"
#include "dataProcessing.h"

void SystemClock_Config(void);


void serial_put_uint(uint16_t number)
{
	uint16_t i;
	uint16_t nTmp;

	for (i = 10000; i >= 1; i = i / 10)

	{
		nTmp = number / i % 10 + 0x30;
		if (nTmp >= 1) HAL_UART_Transmit(&huart1, (uint8_t *)&nTmp, 1, 0xff);
	}
}


int main(void)
{

	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_DAC_Init();
	MX_ADC1_Init();
	MX_USART1_UART_Init();
//	MX_USART2_UART_Init();
	MX_TIM6_Init();
//	MX_TIM7_Init();
//	MX_I2C1_Init();

	uint16_t sendMasterCount;

	while (1)
	{
		if (ReceiveState)
		{	
			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_1);
			ModbusDecode(Usart1ReceiveBuffer.BufferArray, Usart1ReceiveBuffer.BufferLen);
			Usart1ReceiveBuffer.BufferLen = 0;
			ReceiveState = 0;
		}

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

void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


void _Error_Handler(char * file, int line)
{

	while (1)
	{
	}
}


