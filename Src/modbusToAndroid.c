#include "modbusToAndroid.h"
#include "usart.h"

uint8_t SlaveAdd = 1;

uint8_t txBuf[50];
uint8_t txCount = 0;

uint16_t localData[50];


uint16_t GetCRC16(uint8_t *arr_buff, uint8_t len) {  //CRC校验程序
	uint16_t crc = 0xFFFF;
	uint8_t i, j;
	for (j = 0; j < len; j++) {
		crc = crc ^*arr_buff++;
		for (i = 0; i < 8; i++) {
			if ((crc & 0x0001) > 0) {
				crc = crc >> 1;
				crc = crc ^ 0xa001;
			}
			else
				crc = crc >> 1;
		}
	}
	return (crc);
}

void sendDataMaster03() {
	uint16_t temp;
	txBuf[0] = SlaveAdd;
	txBuf[1] = 0x03;
	txBuf[2] = 0x00;
	txBuf[3] = 0x00;
	txBuf[4] = 0x00;
	txBuf[5] = 0x06;//读6位
	temp = GetCRC16(txBuf, 6);
	txBuf[6] = (uint8_t)(temp & 0xff);
	txBuf[7] = (uint8_t)(temp >> 8);
	txCount = 8;
	HAL_UART_Transmit(&huart1, txBuf, txCount, 0xff);
}

void sendDataMaster16() {

	uint16_t temp;
	uint8_t i;

	txBuf[0] = SlaveAdd;
	txBuf[1] = 0x10;
	txBuf[2] = 0x00;         //数据的起始地址；
	txBuf[3] = 0x06;
	txBuf[4] = 0x00;         //数据的个数；
	txBuf[5] = 0x07;
	txBuf[6] = 0x0e;         //数据的字节数；
	for (i = 0; i<txBuf[5]; i++) {
		txBuf[7 + 2 * i] = (uint8_t)(localData[i+ txBuf[3]] >> 8);
		txBuf[8 + 2 * i] = (uint8_t)(localData[i+ txBuf[3]] & 0xff);
	}
	temp = GetCRC16(txBuf, 2 * txBuf[5] + 7);
	txBuf[7 + 2 * txBuf[5]] = (uint8_t)(temp & 0xff);
	txBuf[8 + 2 * txBuf[5]] = (uint8_t)((temp >> 8) & 0xff);
	txCount = 9 + 2 * txBuf[5];
	HAL_UART_Transmit(&huart1, txBuf, txCount, 0xff);
}

void ModbusDecode(uint8_t *MDbuf, uint8_t len) {

	uint16_t  crc;
	uint8_t crch, crcl;
	uint16_t temp;

	if (MDbuf[0] != SlaveAdd) return;								//地址相符时，再对本帧数据进行校验
	crc = GetCRC16(MDbuf, len - 2);								//计算CRC校验值
	crch = crc >> 8;
	crcl = crc & 0xFF;
	if ((MDbuf[len - 1] != crch) || (MDbuf[len - 2] != crcl)) return;	//如CRC校验不符时直接退出
	if (MDbuf[1] != 0x03) return;									//检验功能码
	if (MDbuf[2] > 0x20) return;					//寄存器地址支持0x0000～0x0020
	for (uint8_t i = 0; i < MDbuf[2]/2; i++)
	{
		localData[i] = (uint16_t)(MDbuf[3 + 2*i] << 8) + MDbuf[4 + 2*i];
	}
}

void UsartRxMonitor() {
	static uint8_t ArrayLenTemp;					//存储接收缓存数组上一次长度，用以判断数组长度是否出现变化
	static uint8_t BusIdleCount;					//总线空闲计数
	
	if (Usart1ReceiveBuffer.BufferLen > 0) {
		if (ArrayLenTemp != Usart1ReceiveBuffer.BufferLen) {		//如果此次数组长度与上次不同
			ArrayLenTemp = Usart1ReceiveBuffer.BufferLen;		//此次数组长度赋值给上次
			BusIdleCount = 0;								//清零空闲计时
		}
		else {											//如果此次数组长度与上次相同
			if (BusIdleCount < 4) {
				BusIdleCount++;							//总线空闲计时+1
				if (BusIdleCount >= 4) {					//如果空闲计时>=4
					BusIdleCount = 0;						//空闲计时置零
					HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_1);
					ModbusDecode(Usart1ReceiveBuffer.BufferArray, Usart1ReceiveBuffer.BufferLen);	//解码
					Usart1ReceiveBuffer.BufferLen = 0;											//用完后，缓存数组长度清零
				}
			}
		}
	}
	else ArrayLenTemp = 0;
}
