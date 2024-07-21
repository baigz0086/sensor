/****************************************Copyright (c)*********************************************
**                                     佛山三俊电子有限公司
**									       开   发   部
**--------------文件信息---------------------------------------------------------------------------
**文   件   名: WIFI.c
**创   建   人: 
**创 建 日 期 ：
**描        述: 
**--------------当前版本修订-----------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**************************************************************************************************/
#define	WIFI_GLOBALS

#include "Includes.h"

const  char xdata	APPLY_AT[]="+++";
const  char xdata	APPLY_AT_ACK[]="a";

const  char xdata	ENT_AT[]="a";
const  char xdata	ENT_AT_ACK[]="+ok\r\n\r\n";

const  char xdata	WMAC[]="at+wsmac\r";
const  char xdata	WMAC_ACK[]="AT+WSMAC\n\r+ok=A5A4A3A2A1A0\r\n\r\n";

const  char xdata	WMODE[]="at+wmode=sta\r";
const  char xdata	WMODE_ACK[]="AT+WMODE=sta\n\r+ok\r\n\r\n";

const  char xdata	WNETP[]="at+netp=tcp,client,80,223.134.3.129\r";
const  char xdata	WNETP_ACK[]="AT+NETP=tcp,client,80,223.134.3.129\n\r+ok\r\n\r\n";

// legacy IP 2020/2/20
//const  char xdata	WNETP[]="at+netp=tcp,client,80,47.96.101.117\r";
//const  char xdata	WNETP_ACK[]="AT+NETP=tcp,client,80,47.96.101.117\n\r+ok\r\n\r\n";

const  char xdata	WSLK[]="at+wslk\r";
const  char xdata	WSLK_ACK[]="AT+WSLK\n\r+ok=SSID(MAC)\r\n\r\n";

const  char xdata	TCPON[]="at+tcpdis=on\r";
const  char xdata	TCPON_ACK[]="AT+TCPDIS=on\n\r+ok\r\n\r\n";

const  char xdata	TCPEN[]="at+tcpdis\r";
const  char xdata	TCPEN_ACK[]="AT+TCPDIS\n\r+ok=on\r\n\r\n";

const  char xdata	TCPLK[]="at+tcplk\r";
const  char xdata	TCPLK_ACK[]="AT+TCPLK\n\r+ok=on\r\n\r\n";

const  char xdata	RESET[]="at+z\r";
const  char xdata	RESET_ACK[]="AT+Z\n\r+ok\r\n\r\n";

#define	REC_WIFI_DATA		(Uart2NoRecTime >= 30 && pUART2_rptr >= 1)


void fInitWifi(void)
{
	MacAddr[0] = 0xB1;
	MacAddr[1] = 0xB2;
	MacAddr[2] = 0xB3;
	MacAddr[3] = 0xB4;
	MacAddr[4] = 0xB5;
	MacAddr[5] = 0xB6;
	
	fClean_RecWIFI_Buf();
	StepCommWifi = WIFI_POWERON;
//	StepCommWifi = SERVER_HAND;			//debug
	WaitSendWifi = 600;
}

void fSendServerHand(void)
{
	u16 Sum;
	
	Sum = 0;
	Uart2Send('C');
	Sum += 'C';
	Uart2Send('L');
	Sum += 'L';
	
	Uart2Send(MacAddr[0]);
	Sum += MacAddr[0];
	Uart2Send(MacAddr[1]);
	Sum += MacAddr[1];
	Uart2Send(MacAddr[2]);
	Sum += MacAddr[2];
	Uart2Send(MacAddr[3]);
	Sum += MacAddr[3];
	Uart2Send(MacAddr[4]);
	Sum += MacAddr[4];
	Uart2Send(MacAddr[5]);
	Sum += MacAddr[5];
	
	Uart2Send(12);
	Sum += 12;
	
	Uart2Send('H');
	Sum += 'H';
	
	Uart2Send((unsigned char)(Sum / 256));
	Uart2Send((unsigned char)(Sum));
}

void fSendServerData(void)
{
	u16 Sum;
	u8	i;
	
	Sum = 0;
	Uart2Send('C');
	Sum += 'C';
	Uart2Send('L');
	Sum += 'L';
	
	Uart2Send(MacAddr[0]);
	Sum += MacAddr[0];
	Uart2Send(MacAddr[1]);
	Sum += MacAddr[1];
	Uart2Send(MacAddr[2]);
	Sum += MacAddr[2];
	Uart2Send(MacAddr[3]);
	Sum += MacAddr[3];
	Uart2Send(MacAddr[4]);
	Sum += MacAddr[4];
	Uart2Send(MacAddr[5]);
	Sum += MacAddr[5];
	
	Uart2Send(72);
	Sum += 72;
	
	Uart2Send('4');
	Sum += '4';
	
//		Uart2Send(Year);
//		Sum += Year;
//		Uart2Send(Month);
//		Sum += Month;
//		Uart2Send(Day);
//		Sum += Day;
	Uart2Send(Hour);
	Sum += Hour;
	Uart2Send(Minute);
	Sum += Minute;
	Uart2Send(Second);
	Sum += Second;
	Uart2Send(OTSecond);
	Sum += OTSecond;
	
	for(i=0;i<56;i++)
	{
		Uart2Send(UART1_buffer[i]);
		Sum += UART1_buffer[i];
	}
	
	Uart2Send(Sum / 256);
	Uart2Send(Sum);
}


void fSend_CmdData(void)
{
	u16 Sum;
	u8	i;
	
	Sum = 0;
	Uart2Send('C');
	Sum += 'C';
	Uart2Send('L');
	Sum += 'L';
	
	Uart2Send(MacAddr[0]);
	Sum += MacAddr[0];
	Uart2Send(MacAddr[1]);
	Sum += MacAddr[1];
	Uart2Send(MacAddr[2]);
	Sum += MacAddr[2];
	Uart2Send(MacAddr[3]);
	Sum += MacAddr[3];
	Uart2Send(MacAddr[4]);
	Sum += MacAddr[4];
	Uart2Send(MacAddr[5]);
	Sum += MacAddr[5];
	
	i = 9 + ReplayLen + 2;
	
	Uart2Send(i);
	Sum += i;
	
	for(i=0;i<ReplayLen;i++)
	{
		Uart2Send(UART1_buffer[i]);
		Sum += UART1_buffer[i];
	}
	
	Uart2Send(Sum / 256);
	Uart2Send(Sum);
	
}

u8 fVerifyServerCammand(void)
{
	u8 Command;
	u8 i;
	u16 CheckSum;
	
	Command = 0;
	CheckSum = 0;
	
	for(i = 0; i < (pUART2_rptr - 2); i++) CheckSum += UART2_buffer[i];					//sum data except last two data
	
	if(UART2_buffer[pUART2_rptr - 1] != (unsigned char)(CheckSum)) Command = 0;	//check sum low byte
	else if(UART2_buffer[pUART2_rptr - 2] != (unsigned char)(CheckSum >> 8)) Command = 0; //check sum high byte
	else if(UART2_buffer[0] != 'S' || UART2_buffer[1] != 'V') Command = 0;			//check frame head
	else if(UART2_buffer[2] != MacAddr[0]) Command = 0;													//check ID
	else if(UART2_buffer[3] != MacAddr[1]) Command = 0;
	else if(UART2_buffer[4] != MacAddr[2]) Command = 0;
	else if(UART2_buffer[5] != MacAddr[3]) Command = 0;
	else if(UART2_buffer[6] != MacAddr[4]) Command = 0;
	else if(UART2_buffer[7] != MacAddr[5]) Command = 0;
	else if(UART2_buffer[8] != pUART2_rptr) Command = 0;												//check frame length
	else Command = 1;
	
	Command = 1;
	
	return Command;
}




void fClean_RecWIFI_Buf(void)
{
	u8 i;
	for(i=0;i<70;i++) UART2_buffer[i] = 0;
	Uart2NoRecTime = 0;
	pUART2_rptr = 0;
}

void fSendToWIFI(void)
{
	switch(StepCommWifi)
	{
		case WIFI_APPLY_AT: 
			fClean_RecWIFI_Buf();
			Uart2SendStr(APPLY_AT); 
			StepCommWifi = WIFI_APPLY_AT_ACK;
		break;
			
		case WIFI_ENT_AT:	
			fClean_RecWIFI_Buf();
			Uart2SendStr(ENT_AT); 
			StepCommWifi = WAIT_ENT_AT_ACK;
		break;
		
		case WIFI_WMAC:		
			fClean_RecWIFI_Buf();
			Uart2SendStr(WMAC); 
			StepCommWifi = WIFI_WMAC_ACK;
		break;
		
		case WIFI_WMODE:
			fClean_RecWIFI_Buf();
			Uart2SendStr(WMODE); 
			StepCommWifi = WIFI_WMODE_ACK;
		break;
			
		case WIFI_WNETP:
			fClean_RecWIFI_Buf();
			Uart2SendStr(WNETP); 
			StepCommWifi = WIFI_WNETP_ACK;
		break;
		
		case WIFI_RESET1:	
			fClean_RecWIFI_Buf();
			Uart2SendStr(RESET); 
			StepCommWifi = WIFI_RESET1_ACK;	
		break;
		
		case WIFI_APPLY1_AT: 
			fClean_RecWIFI_Buf();
			Uart2SendStr(APPLY_AT); 
			StepCommWifi = WIFI_APPLY1_AT_ACK;
		break;
			
		case WIFI_ENT1_AT:	
			fClean_RecWIFI_Buf();
			Uart2SendStr(ENT_AT); 
			StepCommWifi = WAIT_ENT1_AT_ACK;
		break;
		
		case WIFI_WSLK:
			fClean_RecWIFI_Buf();
			Uart2SendStr(WSLK); 
			StepCommWifi = WIFI_WSLK_ACK;
		break;
		
		case WIFI_TCPON:	
			fClean_RecWIFI_Buf();
			Uart2SendStr(TCPON); 
			StepCommWifi = WIFI_TCPON_ACK;
		break;
		
		case WIFI_TCPEN:	
			fClean_RecWIFI_Buf();
			Uart2SendStr(TCPEN); 
			StepCommWifi = WIFI_TCPEN_ACK;
		break;
		
		case WIFI_TCPLK:	
			fClean_RecWIFI_Buf();
			Uart2SendStr(TCPLK); 
			StepCommWifi = WIFI_TCPLK_ACK;
		break;
		
		case WIFI_RESET2:	
			fClean_RecWIFI_Buf();
			Uart2SendStr(RESET); 
			StepCommWifi = WIFI_RESET2_ACK;	
		break;
		
		case INIT_WIFI:							//wifi模块初始化复位成功后进入INIT_WIFI状态
			if(StepCommSensor == IDLE_SENSOR)	//if sensor ready,do shake hand
			{
				StepCommWifi = SERVER_HAND;
			}
		break;
			
		case SERVER_HAND:						//shake hand and wait for ack
			fClean_RecWIFI_Buf();
			fSendServerHand();
			StepCommWifi = SERVER_HAND_ACK;
		break;	
			
		// wifi send 
		
		case WIFI_IDLE:							//after success shake hand enter idle status
			if(bSend_WIFI_Data) 			//set after recevive sensor data
			{
				bSend_WIFI_Data = 0;		//surrounding parameter send to server
				fSendServerData();
			}
		break;
			
		case REC_SERVER_COMMAND:		//success for received command of server, wait for reply data
			if(bServer_Cmd_Ack && ReplayServerCount == 0)
			{
				bServer_Cmd_Ack = 0;
				fSend_CmdData();
				StepCommWifi = WIFI_IDLE;
			}					
		break;
	}
}
		
void fRecWifi(void)
{
	
	switch(StepCommWifi)
	{
		case WIFI_APPLY_AT_ACK: 
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[0] == 'a') 
				{
					StepCommWifi = WIFI_ENT_AT;
				}
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_APPLY_AT;
			}
		break;
	
		case WAIT_ENT_AT_ACK: 	
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[0] == '+' && UART2_buffer[1] == 'o' && UART2_buffer[2] == 'k') 
				{
					StepCommWifi = WIFI_WMAC;
				}
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_APPLY_AT;
			}
		break;
		
		case WIFI_WMAC_ACK: 	
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[10] == '+' && UART2_buffer[11] == 'o' && UART2_buffer[12] == 'k') 
				{	
					MacAddr[0] = IntegrateAscToHex(UART2_buffer[14],UART2_buffer[15]);
					MacAddr[1] = IntegrateAscToHex(UART2_buffer[16],UART2_buffer[17]);
					MacAddr[2] = IntegrateAscToHex(UART2_buffer[18],UART2_buffer[19]);
					MacAddr[3] = IntegrateAscToHex(UART2_buffer[20],UART2_buffer[21]);
					MacAddr[4] = IntegrateAscToHex(UART2_buffer[22],UART2_buffer[23]);
					MacAddr[5] = IntegrateAscToHex(UART2_buffer[24],UART2_buffer[25]);
					
					StepCommWifi = WIFI_WMODE;
				}	
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_WMAC;
			}
		break;
			
		case WIFI_WMODE_ACK:
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[14] == '+' && UART2_buffer[15] == 'o' && UART2_buffer[16] == 'k') 
				{
					StepCommWifi = WIFI_WNETP;
				}
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_WMODE;
			}
		break;
			
		case WIFI_WNETP_ACK:
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[37] == '+' && UART2_buffer[38] == 'o' && UART2_buffer[39] == 'k') 
				{
					StepCommWifi = WIFI_RESET1;
				}
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_WNETP;
			}
		break;
			
		case WIFI_RESET1_ACK:
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[6] == '+' && UART2_buffer[7] == 'o' && UART2_buffer[8] == 'k') 
				{
					WaitSendWifi = 150;
					StepCommWifi = WIFI_APPLY1_AT;
				}	
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_RESET1;
			}
		break;
			
		case WIFI_APPLY1_AT_ACK: 
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[0] == 'a') 
				{
					StepCommWifi = WIFI_ENT1_AT;
				}
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_APPLY1_AT;
			}
		break;
	
		case WAIT_ENT1_AT_ACK: 	
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[0] == '+' && UART2_buffer[1] == 'o' && UART2_buffer[2] == 'k') 
				{
					WaitSendWifi = 600;
					StepCommWifi = WIFI_WSLK;
				}
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_APPLY1_AT;
			}
		break;	
			
		case WIFI_WSLK_ACK:
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[13] == 'D' && UART2_buffer[14] == 'i' && UART2_buffer[15] == 's')
				{
					WaitSendWifi = 10;
					StepCommWifi = WIFI_WSLK;
				}
				else if(UART2_buffer[9] == '+' && UART2_buffer[10] == 'o' && UART2_buffer[11] == 'k')
				{
					StepCommWifi = WIFI_TCPON;
				}
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_WSLK;
			}
		
		break;
			
		case WIFI_TCPON_ACK: 	
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[14] == '+' && UART2_buffer[15] == 'o' && UART2_buffer[16] == 'k') 
				{
					StepCommWifi = WIFI_TCPEN;
				}	
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_TCPON;
			}
		break;	
			
		case WIFI_TCPEN_ACK: 	
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[14] == '=' && UART2_buffer[15] == 'o' && UART2_buffer[16] == 'n') 
				{
					StepCommWifi = WIFI_TCPLK;
				}	
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_TCPEN;
			}
		break;		
			
		case WIFI_TCPLK_ACK: 	
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[13] == '=' && UART2_buffer[14] == 'o' && UART2_buffer[15] == 'n') 
				{
					StepCommWifi = WIFI_RESET2;
				}	
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_TCPLK;
			}
		break;	
			
		case WIFI_RESET2_ACK: 	
			if(REC_WIFI_DATA) 
			{
				if(UART2_buffer[6] == '+' && UART2_buffer[7] == 'o' && UART2_buffer[8] == 'k') 
				{
					WaitSendWifi = 150;
					StepCommWifi = INIT_WIFI;
				}	
			}
			else if(Uart2NoRecTime >= 100) 
			{
				WaitSendWifi = 100;
				StepCommWifi = WIFI_RESET2;
			}
		break;		
				
		case SERVER_HAND_ACK:
			if(REC_WIFI_DATA) 
			{
				if(fVerifyServerCammand() && UART2_buffer[9] == 'H') 	//53 56 B1 B2 B3 B4 B5 B6 0C 48 05 32
				{
					StepCommWifi = WIFI_IDLE;
				}
				else
				{
					StepCommWifi = SERVER_HAND;
				}
			}
			else if(Uart2NoRecTime >= 300) 
			{
				StepCommWifi = SERVER_HAND;
			}
		break;
			
		// wifi receive
		// As long as the command is received, no matter what the status is Enter send mode	
			
		case WIFI_IDLE:							//only wait for receive server command
			if(REC_WIFI_DATA) 
			{
				if(fVerifyServerCammand() && UART2_buffer[8] == 0x0E)		//check frame and frame length, 14 bytes for server command
				{
					ServerCommand = UART2_buffer[9];
					ReplyTime = UART2_buffer[10];						//reply until this time count down
					ReplayLen = UART2_buffer[11];						//minimum length of reply		
					ReplayServerCount = ReplyTime * 10;
					
					StepCommWifi = REC_SERVER_COMMAND;			//turn to command mode
				}
			}	
			else if(Uart2NoRecTime >= 100)  fClean_RecWIFI_Buf();
		
		break;
			
	}
}
		

//unsigned char AscToHex(unsigned char aHex)
//	{
//		if((aHex>=0)&&(aHex<=9))
//		aHex += 0x30;
//		else if((aHex>=10)&&(aHex<=15))//A-F
//		aHex += 0x37;
//		else aHex = 0xff;
//		return Hex;
//	}

unsigned char HexToAsc(unsigned char aChar)
{
	if((aChar>=0x30)&&(aChar<=0x39))
	aChar -= 0x30;
	else if((aChar>=0x41)&&(aChar<=0x46))//capital
	aChar -= 0x37;
	else if((aChar>=0x61)&&(aChar<=0x66))//small
	aChar -= 0x57;
	else aChar = 0xff;
	return aChar;
}		
	
unsigned char IntegrateAscToHex(unsigned char High_aChar,unsigned char Low_aChar)
{
	unsigned char Hex;
	Hex = (HexToAsc(High_aChar) << 4) + HexToAsc(Low_aChar);
	return Hex;
}	