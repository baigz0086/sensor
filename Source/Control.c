/****************************************Copyright (c)*********************************************
**                                     佛山三俊电子有限公司
**									       开   发   部
**--------------文件信息---------------------------------------------------------------------------
**文   件   名: Control.c
**创   建   人: 
**创 建 日 期 ：
**描        述: 
**--------------当前版本修订-----------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**************************************************************************************************/
#define	CTRL_GLOBALS

#include "Includes.h"


/**************************************************************************************************
**	函数名称:	InitPort()
**	功	能	:	初始化IO口 
**	入口参数:	无 
**	出口参数:	无 
**  返回值  ：  无 
**	全局变量:	无 
**	调用模块:	无 
**	作	者	:	
**	日	期	:	
**************************************************************************************************/
void InitPort(void)
{

//	P1   = 0x01;			//no receive in uart2 when set this register
	P1M1 = 0; 
	P1M0 = 0;
	
	P3   = 0x43;
	P3M1 = 0x40; 
	P3M0 = 0x40;
	P3PU &= ~0x40;
	
	P5   = 0x10;
	P5M1 = 0x00; 
	P5M0 = 0x00;

}

/**************************************************************************************************
**	函数名称:	WatchdogInit()
**	功	能	:	看门狗初始化 
**	入口参数:	无 
**	出口参数:	无 
**  返回值  ：无 
**	全局变量:	无 
**	调用模块:	无 
**	作	者	:
**	日	期	:	
**************************************************************************************************/
void WatchdogInit(void)
{

	WDT_CONTR = 0x04;       //看门狗定时器溢出时间计算公式: (12 * 32768 * PS) / FOSC (秒)
                            //设置看门狗定时器分频数为32,溢出时间如下:
                            //11.0592M : 1.14s
                            //18.432M  : 0.68s
                            //20M      : 0.63s
	WDT_CONTR |= 0x20;      //启动看门狗 

}

//u8 Match;

void Control_Init(void)
{	
	EA = 0;									/* Interrupt disable */	 
	
	InitPort();   
	WatchdogInit();
	Time0_Init();
	Uart1Init();			//sence p30 rx p31 tx
	Uart2Init();			//wifi p10 rx p11 tx
//	Uart3Init();			//wifi p00 rx p01 tx
//	Uart4Init();			//sence p02 rx p03 tx
	
	fClean_RecSensor_Buf();
	WaitSensorData = 1000;
	StepCommSensor = HAND_SENSOR;
	
	DayOfMon[0] = 31;
	DayOfMon[1] = 28;
	DayOfMon[2] = 31;
	DayOfMon[3] = 30;
	DayOfMon[4] = 31;
	DayOfMon[5] = 30;
	DayOfMon[6] = 31;
	DayOfMon[7] = 31;
	DayOfMon[8] = 30;
	DayOfMon[9] = 31;
	DayOfMon[10] = 30;
	DayOfMon[11] = 31;
	
	Year = 0;
	Month = 1;
	Day = 1;
	Hour = 0;
	Minute = 0;
	Second = 0;
	OTSecond = 0;
	
	fInitWifi();

	oLED = 1;
	
	EA = 1;											/* Interrupt enable */
//	Uart1SendStr("Uart Test !\r\n");
}


void fKey_reload(void)
{
	if(ilKey)			// key press
	{
		olReload = 0;
	}
	else
	{
		olReload = 1;
	}
}

void ConFunction(void)
{
	WDT_CONTR |= 0x10;
	
	fKey_reload();
	
	if(bDo10ms) Do10ms();
	if(bDo100ms) Do100ms();
	
	if(WaitSensorData == 0) fSendToSensor();
	fRecSensor();
	
	if(WaitSendWifi == 0) fSendToWIFI();
	fRecWifi();
	
}




void fClean_RecSensor_Buf(void)
{
	u8 i;
	for(i=0;i<70;i++) UART1_buffer[i] = 0;
	pUART1_rptr = 0;		
	Uart1NoRecTime = 0;	
}

void fSendToSensor(void)
{
	switch(StepCommSensor)
	{
		case HAND_SENSOR: 						//shake hand sensor first
			fClean_RecSensor_Buf();
			Uart1Send('0'); 
			StepCommSensor = WAIT_HAND_SENSOR;
		break;
	
		case HAND2_SENSOR:						//shake hand sensor second
			fClean_RecSensor_Buf();
			Uart1Send('0'); 
			StepCommSensor = WAIT_HAND2_SENSOR;
		break;
		
		case STUDY_SENSOR:						//study continue 30 second
			fClean_RecSensor_Buf();
			Uart1Send('G'); 
			StepCommSensor = WAIT_STUDY_SENSOR;
		break;
		
		case SAVE_SENSOR:						//save surrounding parameter
			fClean_RecSensor_Buf();
			Uart1Send('S'); 
			StepCommSensor = WAIT_SAVE_SNESOR;
		break;
		
		case IDLE_SENSOR:						//sensor ready
			if(StepCommWifi == WIFI_IDLE)		//no receive server command,then query data	
			{
				fClean_RecSensor_Buf();
				Uart1Send('3'); 
				StepCommSensor = WAIT_READY_SENSOR;
			}
			else if(StepCommWifi == REC_SERVER_COMMAND)
			{
				StepCommSensor = SEND_SENSOR_COMMAND;	// if receive server command,and no in normal data mode,
			}																				// turn to send sensor command
		break;

		case READY_SENSOR:
			fClean_RecSensor_Buf();
			Uart1Send('3'); 
			StepCommSensor = WAIT_READY_SENSOR;
		break;
		
		case DATA_SENSOR:	
			fClean_RecSensor_Buf();
			Uart1Send('4'); 
			StepCommSensor = WAIT_DATA_SENSOR;
		break;
		
		case SEND_SENSOR_COMMAND:
			fClean_RecSensor_Buf();
			bServer_Cmd_Ack = 0;
			Uart1Send(ServerCommand); 	
			StepCommSensor = WAIT_SENSOR_CMDACK;	// turn to wait command data of sensor
		break;
		
	}
}


// 上电4.6秒SENSOR发送 "AT+WRUART=57600，1，N\r\n"

void fRecSensor(void)
{
	switch(StepCommSensor)
	{
		case WAIT_HAND_SENSOR: 	
			if(Uart1NoRecTime >= 5 && pUART1_rptr >= 1) 
			{
				if(UART1_buffer[0] == '1') 
				{
					StepCommSensor = HAND2_SENSOR;	
					WaitSensorData = 500;
				}
			}
			else if(Uart1NoRecTime >= 100)  StepCommSensor = HAND_SENSOR;
		break;
	
		case WAIT_HAND2_SENSOR: 
			if(Uart1NoRecTime >= 5 && pUART1_rptr >= 1) 
			{	
				if(UART1_buffer[0] == '1') 
				{
					StepCommSensor = STUDY_SENSOR;	
					WaitSensorData = 500;
				}
			}
			else if(Uart1NoRecTime >= 100)  StepCommSensor = HAND_SENSOR;
		break;	
			
		case WAIT_STUDY_SENSOR:	
			if(Uart1NoRecTime >= 5 && pUART1_rptr >= 1) 
			{
				if(UART1_buffer[0] == '1') StepCommSensor = SAVE_SENSOR;
			}
			else if(Uart1NoRecTime >= 3500) StepCommSensor = HAND_SENSOR;
		break;
		
		case WAIT_SAVE_SNESOR:	
			if(Uart1NoRecTime >= 5 && pUART1_rptr >= 1) 
			{
				if(UART1_buffer[0] == '1') StepCommSensor = IDLE_SENSOR;
			}
			else if(Uart1NoRecTime >= 150)  StepCommSensor = HAND_SENSOR;
		break;
			
		case WAIT_READY_SENSOR:			//normal qurey data ack
			if(Uart1NoRecTime >= 5 && pUART1_rptr >= 1) 
			{
				if(UART1_buffer[0] == '1') StepCommSensor = DATA_SENSOR;
			}
			else if(Uart1NoRecTime >= 100)  
			{
				StepCommSensor = IDLE_SENSOR;	
			}
		break;
		
		case WAIT_DATA_SENSOR:			//normal receive data
			if(Uart1NoRecTime >= 5 && pUART1_rptr >= 31) 
			{
				StepCommSensor = IDLE_SENSOR;
				WaitSensorData = 84;

				bSend_WIFI_Data = 1;
			}
			else if(Uart1NoRecTime >= 100) StepCommSensor = IDLE_SENSOR;
		break;
		
		case WAIT_SENSOR_CMDACK:		//receive command ack data
			if(Uart1NoRecTime >= 5 && pUART1_rptr >= 1) 
			{
				bServer_Cmd_Ack = 1;
				WaitSensorData = 10;
				StepCommSensor = IDLE_SENSOR;
			}
			else if(Uart1NoRecTime >= 3500) 	//if no receive data in 35 seconds, return to idle
			{
				StepCommSensor = IDLE_SENSOR;	
				StepCommWifi = WIFI_IDLE;
			}
		break;


	}
}






void fTime(void)
{
	unsigned char Temp;
	if(OTSecond < 9) OTSecond++;
	else
	{
		OTSecond = 0;
		if(Second < 59) Second++;
		else
		{
			Second = 0;
			if(Minute < 59) Minute++;
			else
			{
				Minute = 0;
				if(Hour < 23) Hour++;
				else
				{
					Hour = 0;
					if(Month == 2)
					{
					//	if((Year%4==0 && Year%100!=0)||(Year%400==0)) Temp = 29;
						if(Year%4==0 && Year%100!=0) Temp = 29;
						else Temp = 28;
						
						if(Day < Temp) Day++;
						else
						{
							Day = 1;
							Month = 3;
						}
					}
					else
					{
						Temp = DayOfMon[Month - 1];
						if(Day < Temp) Day++;
						else
						{
							Day = 1;
							if(Month < 12) Month++;
							else
							{
								Month = 1;
								Year++;
							}
						}
					}
				}
			}
		}		
	}
}


