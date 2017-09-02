#ifndef __MODBUSTOANDROID_
#define __MODBUSTOANDROID_

#include "stm32f1xx_hal.h"
#include "main.h"

extern unsigned short localData[];

void UsartRxMonitor();
void sendDataMaster03();
void sendDataMaster16();

#endif // __MODBUSTOANDROID_
