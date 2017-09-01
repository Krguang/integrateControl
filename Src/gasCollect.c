#include "gasCollect.h"
#include "adc.h"

unsigned short ch0temp = 0;


void gasCollect() {
	ch0temp= Get_Adc_Average(ADC_CHANNEL_0,10);
}