
#include "tim.h"

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

uint16_t tim6Count = 0;
uint16_t tim7Count = 0;

volatile uint8_t tim6Flag = 0;
volatile uint8_t tim7Flag = 0;


void MX_TIM6_Init(void)
{
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 9;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 7199;
	HAL_TIM_Base_Init(&htim6);
	HAL_TIM_Base_Start_IT(&htim6);
}

void MX_TIM7_Init(void)
{
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = 9999;
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 7199;
	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start_IT(&htim7);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

	if (tim_baseHandle->Instance == TIM6)
	{
		__HAL_RCC_TIM6_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM6_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM6_IRQn);
	}
	else if (tim_baseHandle->Instance == TIM7)
	{
		__HAL_RCC_TIM7_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM7_IRQn);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == (&htim6))
	{
		tim6Count++;
		if (tim6Count>5)
		{
			tim6Count = 0;
			tim6Flag = 1;
		}
		
	}
	else if (htim == (&htim7)) {
		
	}
}
