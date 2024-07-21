/****************************************Copyright (c)*********************************************
**                                     ��ɽ�����������޹�˾ 
**									       ���ܵ�����ҵ�� 
**--------------�ļ���Ϣ---------------------------------------------------------------------------
**��   ��   ��: Control.h
**��   ��   ��: 
**�� �� �� �� �� 
**��        ��: ���ܿ��Ƴ��� 
**--------------��ǰ�汾�޶�-----------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**************************************************************************************************/
#ifdef	CTRL_GLOBALS

#define	CTRL_EXT	
#else
#define	CTRL_EXT  extern
#endif


//	M1 M0
//   0  0	˫���
//   0  1	�������
//   1  0	����
//   1  1	��©


/*
P1-7		olReload
   6		
   5    olWPS
   4
   3
   2
   1    oWIFI_Tx
   0    iWIFI_Rx

	P1   = 0xA1;
	P1M1 = 0; 
	P1M0 = 0xA2;

P3-7		
   6    ilKey
	 5		
	 4
	 3
	 2
	 1		oMCU_Tx
	 0		iMCU_Rx
	 
	P3   = 0x41;
	P3M1 = 0; 
	P3M0 = 0x02;
	 
P5-5		
	 4		olReset
	 
	P5   = 0x10;
	P5M1 = 0x00; 
	P5M0 = 0x10;
	 
*/

sbit	olReload = P1^7;
sbit	olWPS = P1^5;
sbit	ilKey = P3^6;
sbit	olReset = P5^4;
sbit	oTx = P3^1;
sbit	oP32 = P3^2;

sbit oLED	= P5^5;


typedef enum
{
	HAND_SENSOR					,
	WAIT_HAND_SENSOR		,
	HAND2_SENSOR				,
	WAIT_HAND2_SENSOR		,
	STUDY_SENSOR				,
	WAIT_STUDY_SENSOR		,
	SAVE_SENSOR					,
	WAIT_SAVE_SNESOR		,
	READY_SENSOR				,
	WAIT_READY_SENSOR		,
	IDLE_SENSOR					,
	SEND_SENSOR_COMMAND	,	//send command to sensor after receive server command
	WAIT_SENSOR_CMDACK	,	//wait for sensor data of command
	DATA_SENSOR					,	//send normal surrounding parameter data ask
	WAIT_DATA_SENSOR			//wait for data
}eStepCommSensor;

CTRL_EXT eStepCommSensor StepCommSensor;



CTRL_EXT	BOOL	bSend_Sensor_Interval;
CTRL_EXT	BOOL	bWaitRecSensor;
CTRL_EXT	u8	ResendSensorTime;		//��10Ms������ʱ1S
CTRL_EXT	u8	ResendSensorCount;		
CTRL_EXT	u16	RecSensorOverTime;
CTRL_EXT	u8	AlarmRecSensor;

CTRL_EXT	u16	Uart1NoRecTime;
CTRL_EXT	u16	WaitSensorData;
CTRL_EXT	BOOL	bSend_WIFI_Data;

CTRL_EXT	BOOL	bServer_Cmd_Ack;

CTRL_EXT	u16	Uart2NoRecTime;

typedef enum
{
	WIFI_POWERON				,			//�ϵ�1���Ӻ�ʼ��ʼ��WIFIģ��
	WIFI_APPLY_AT				,			//�����������ģʽ
	WIFI_APPLY_AT_ACK		,			//����ģʽ�ظ�
	WIFI_ENT_AT					,			//��������ģʽ
	WAIT_ENT_AT_ACK			,			//��������ģʽ�ظ�
	WIFI_WMAC						,			//��ѯMAC
	WIFI_WMAC_ACK				,			//��ѯMAC�ظ�
	WIFI_WMODE					,			//set wifi mode
	WIFI_WMODE_ACK			,			//set wifi mode reply
	WIFI_WNETP					,			//set tcp id and terminal port
	WIFI_WNETP_ACK			,			//set tcp id and terminal port reply
	WIFI_RESET1					, 		//WIFIģ������1
	WIFI_RESET1_ACK			, 		//WIFIģ������1�ظ�
	WIFI_APPLY1_AT			, 		//apply enter set mode again
	WIFI_APPLY1_AT_ACK	, 		//apply enter set mode again reply
	WIFI_ENT1_AT				,			//enter set mode again
	WAIT_ENT1_AT_ACK		,			//enter set mode again reply
	WIFI_WSLK						,			//��ѯWIFI����״̬
	WIFI_WSLK_ACK				,			//��ѯWIFI����״̬�ظ�
	WIFI_TCPON					,			//���ÿ���TCP
	WIFI_TCPON_ACK			,			//���ÿ���TCP�ظ�
	WIFI_TCPEN					,			//��ѯTCP����״̬
	WIFI_TCPEN_ACK			,			//��ѯTCP����״̬�ظ�
	WIFI_TCPLK					,			//��ѯTCP����״̬
	WIFI_TCPLK_ACK			,			//��ѯTCP����״̬�ظ�
	WIFI_RESET2					,			//WIFIģ������2
	WIFI_RESET2_ACK			,			//WIFIģ������2�ظ�
	INIT_WIFI						,			//���WiFi��ʼ��
	SERVER_HAND					,	
	SERVER_HAND_ACK			,	
	WIFI_IDLE						,	
	REC_SERVER_COMMAND
}nStepCommWifi;

CTRL_EXT	nStepCommWifi	StepCommWifi;

CTRL_EXT	u16	WaitSendWifi;
CTRL_EXT	u8	ServerCommand,ReplyTime,ReplayLen;
CTRL_EXT	u16	ReplayServerCount;	
CTRL_EXT	u8	MacAddr[6];


CTRL_EXT INT8U const ENTATP[];


//ʱ��

//CTRL_EXT	u8	DayOfMon[12]={31,28,31,30,31,30,31,31,30,31,30,31};
CTRL_EXT	u8	DayOfMon[12];
CTRL_EXT	u8	Year;
CTRL_EXT	u8	Month;
CTRL_EXT	u8	Day;
CTRL_EXT	u8	Hour;
CTRL_EXT	u8	Minute;
CTRL_EXT	u8	Second;
CTRL_EXT	u8	OTSecond;



void Control_Init(void);				//���ܳ�ʼ��
void ConFunction(void);					//���ܲ���
void fSendToSensor(void);
void fRecSensor(void);
void fClean_RecSensor_Buf(void);
void fTime(void);


