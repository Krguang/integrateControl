
#include "adc.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */

uint16_t ADC_ConvertedValue[7];
uint32_t ADC_Average[7];
/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{
	ADC_ChannelConfTypeDef sConfig;

	/**Common config
	*/
	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 7;
	HAL_ADC_Init(&hadc1);

	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = 2;
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = 3;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = 4;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = 5;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	sConfig.Channel = ADC_CHANNEL_7;
	sConfig.Rank = 6;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	sConfig.Channel = ADC_CHANNEL_8;
	sConfig.Rank = 7;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	if (adcHandle->Instance == ADC1)
	{
		/* USER CODE BEGIN ADC1_MspInit 0 */

		/* USER CODE END ADC1_MspInit 0 */
		/* ADC1 clock enable */
		__HAL_RCC_ADC1_CLK_ENABLE();

		/**ADC1 GPIO Configuration
		PA0-WKUP     ------> ADC1_IN0
		PA1     ------> ADC1_IN1
		PA2     ------> ADC1_IN2
		PA3     ------> ADC1_IN3
		PA6     ------> ADC1_IN6
		PA7     ------> ADC1_IN7
		PB0     ------> ADC1_IN8
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
			| GPIO_PIN_6 | GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_0;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* ADC1 DMA Init */
		/* ADC1 Init */
		hdma_adc1.Instance = DMA1_Channel1;
		hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_adc1.Init.Mode = DMA_CIRCULAR;
		hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
		HAL_DMA_Init(&hdma_adc1);

		__HAL_LINKDMA(adcHandle, DMA_Handle, hdma_adc1);

		/* USER CODE BEGIN ADC1_MspInit 1 */



		/* USER CODE END ADC1_MspInit 1 */
	}
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

	if (adcHandle->Instance == ADC1)
	{
		/* USER CODE BEGIN ADC1_MspDeInit 0 */

		/* USER CODE END ADC1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_ADC1_CLK_DISABLE();

		/**ADC1 GPIO Configuration
		PA0-WKUP     ------> ADC1_IN0
		PA1     ------> ADC1_IN1
		PA2     ------> ADC1_IN2
		PA3     ------> ADC1_IN3
		PA6     ------> ADC1_IN6
		PA7     ------> ADC1_IN7
		PB0     ------> ADC1_IN8
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
			| GPIO_PIN_6 | GPIO_PIN_7);

		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);

		/* ADC1 DMA DeInit */
		HAL_DMA_DeInit(adcHandle->DMA_Handle);
		/* USER CODE BEGIN ADC1_MspDeInit 1 */

		/* USER CODE END ADC1_MspDeInit 1 */
	}
}

