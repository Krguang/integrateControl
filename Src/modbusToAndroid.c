#include "modbusToAndroid.h"
#include "usart.h"

unsigned char SlaveAdd = 1;
unsigned char alarmStatus[7] = { 1,2,3,4,5,6,7 };
unsigned char LocalStatusArray[50];

unsigned int GetCRC16(unsigned char *arr_buff, unsigned char len) {  //CRC校验程序
	unsigned int crc = 0xFFFF;
	unsigned char i, j;
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


void ModbusDecode(unsigned char *MDbuf, unsigned char len) {

	unsigned char i;
	unsigned char cnt;
	unsigned int  crc;
	unsigned char crch, crcl;

	if (MDbuf[0] != SlaveAdd) return;								//地址相符时，再对本帧数据进行校验
	crc = GetCRC16(MDbuf, len - 2);								//计算CRC校验值
	crch = crc >> 8;
	crcl = crc & 0xFF;
	if ((MDbuf[len - 1] != crch) || (MDbuf[len - 2] != crcl)) return;	//如CRC校验不符时直接退出

	switch (MDbuf[1]) {											//地址和校验字均相符后，解析功能码，执行相关操作

	case 0x03:											//读取一个或连续的寄存器
		if ((MDbuf[2] == 0x00) && (MDbuf[3] <= 0x20)) {			//只支持0x0000～0x0020
			i = MDbuf[3];									//提取寄存器地址
			cnt = MDbuf[5];									//提取待读取的寄存器数量
			MDbuf[2] = cnt * 2;								//读取数据的字节数，为寄存器数*2
			len = 3;										//帧前部已有地址、功能码、字节数共3个字节
			while (cnt--) {
				unsigned int LocalStatusArrayTemp = LocalStatusArray[i++];	//读取的是16位数组，转换为2个8位数据存入发送数组
				MDbuf[len++] = LocalStatusArrayTemp >> 8;
				MDbuf[len++] = LocalStatusArrayTemp & 0xff;
			}
		}
		else {					//寄存器地址不被支持时，返回错误码
			MDbuf[1] = 0x83;	//功能码最高位置1
			MDbuf[2] = 0x02;	//设置异常码为02-无效地址
			len = 3;
		}
		break;

	case 0x06:											//写入单个寄存器
		if ((MDbuf[2] == 0x00) && (MDbuf[3] <= 0x20)) {	//寄存器地址支持0x0000～0x0020
			i = MDbuf[3];								//提取寄存器地址
			LocalStatusArray[i] = MDbuf[5];				//保存寄存器数据
			len -= 2;									//长度-2以重新计算CRC并返回原帧
		}
		else {					//寄存器地址不被支持时，返回错误码{
			MDbuf[1] = 0x86;	//功能码最高位置1
			MDbuf[2] = 0x02;	//设置异常码为02-无效地址
			len = 3;
		}
		break;

	case 0x10:
		if ((MDbuf[2] == 0x00) && (MDbuf[3] <= 0x20)) {		//寄存器地址支持0x0000～0x0020
			i = MDbuf[3];									//提取寄存器地址
			cnt = MDbuf[5];									//提取待写入的寄存器数量
			unsigned char startNum = 7;						//设置读取将要写入的数据的地址
			unsigned int writeTemp = 0;						//写两次8位转16位的缓存
			while (cnt--) {									//写cnt次
				writeTemp = MDbuf[startNum];					//第一位数据写入缓存
				LocalStatusArray[i] = (writeTemp << 8) + MDbuf[startNum + 1];	//保存寄存器数据
				i++;
				startNum = startNum + 2;
			}

			len = 6;			//保留6帧重新计算CRC并返回原帧
		}
		else {					//寄存器地址不被支持时，返回错误码{
			MDbuf[1] = 0x86;	//功能码最高位置1
			MDbuf[2] = 0x02;	//设置异常码为02-无效地址
			len = 3;
		}
		break;

	default:					//其它不支持的功能码
		MDbuf[1] |= 0x80;		//功能码最高位置1
		MDbuf[2] = 0x01;		//设置异常码为01-无效功能
		len = 3;
		break;
	}
	crc = GetCRC16(MDbuf, len);		//计算返回帧的CRC校验值
	MDbuf[len++] = crc & 0xFF;		//CRC低字节
	MDbuf[len++] = crc >> 8;		//CRC高字节
									//UsartTransmitArray(MDbuf, len);	//发送返回帧
	HAL_UART_Transmit(&huart1, MDbuf, len, 1000);
}

void UsartRxMonitor() {
	static unsigned char ArrayLenTemp;					//存储接收缓存数组上一次长度，用以判断数组长度是否出现变化
	static unsigned char BusIdleCount;					//总线空闲计数
	
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
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
					ModbusDecode(Usart1ReceiveBuffer.BufferArray, Usart1ReceiveBuffer.BufferLen);	//解码
					Usart1ReceiveBuffer.BufferLen = 0;											//用完后，缓存数组长度清零
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
				}
			}
		}
	}
	else ArrayLenTemp = 0;
}
