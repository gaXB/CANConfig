/************************�ļ�˵��*****************************************************
* Copyright (c) 2019 ,���ݹ㰲�����������޹�˾  All rights reserved.
*
* �ļ����ƣ�InterLayMatrix.h
* ����������InterLay�����ݷ��ͣ�Ĭ��ֵ��timeoutֵ�����ͽ��ձ�Ķ��塣�밴�淶��д
* �������� codewarrior IDE 11.1 
* MCU˵������ģ��Ϊ�ϲ�Ӧ�ã���MCU �޹�
* 
*���ļ���PC �Զ����ɣ�PC �汾 V1.0, DBC �ļ�����  CVTC_SV51-02&B1_CMX_20200409_BD_V4.81 - HCM.dbc
* �������ڣ�  07/06/20 14:06:28
/**************************���ļ�¼******************************************************
 *
* �汾��1.0
* ˵������ʼ�汾����
* ԭ���ߣ�
* ������ڣ�
*/
#ifndef _INTERLAY_MATRIX_H_
#define _INTERLAY_MATRIX_H_
#include "timerout.h"
#include "CANIDConfig.h"


//CAN�������ݽṹ��
typedef struct
{
           uint8   IfID;            //if��ID
  volatile uint8   *pMsg;        //��������(���Ĵ洢��ַ)
           uint16  Cycle;        //���ķ�������(��)
  volatile uint8   *pStatus;     //����״̬
  const    uint8   *pInit;       //���ĳ�ʼ��ֵ
}CANMsgTXAttrStruct;

//CAN�������ݽṹ��
typedef struct
{
           uint32  ID;           //����Ϊ����ID
  volatile uint8   *pMsg;        //��������(���Ĵ洢��ַ)
           uint16  Cycle;        //���ĳ�ʱ����(��)
  volatile uint8   *pStatus;     //����״̬
  const    uint8   *pInit;       //���ĳ�ʼ��ֵ
  const    uint8   *pTimeOut;    //���ĵ���ֵ
  const    uint8   *pMask;       //��������λ(����ĳһλ�ڵ���ʱ�Ƿ����Ϊ����ֵ)
}CANMsgRXAttrStruct;

///��ʼ��Ϊ0�����ı��ֵ�����û�ж�������������ΪĬ��ֵ
static const uint8  CANMsgZEROInit[8]    = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8  CANMsgUNCTimeout[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8  CANMsgUNCMask[8]    = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const uint8  CANMsgZEROMask[8]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//
//MSGC9 ����
typedef union 
 	struct
	{
		unsigned	Reserved_0  :8;	 //Ԥ��_0
		unsigned	Reserved_1  :8;	 //Ԥ��_1
		unsigned	Reserved_2  :8;	 //Ԥ��_2
		unsigned	Reserved_3  :8;	 //Ԥ��_3
		unsigned	Reserved_4  :8;	 //Ԥ��_4
		unsigned	Reserved_5  :8;	 //Ԥ��_5
		unsigned	Reserved_6  :3;	 //Ԥ��_6
		unsigned	PrplsnSysAtv  :1;	 //Power Status: Ready or Not Ready����ǿ�ƣ�Propulsion System Active�� 
		unsigned	Reserved_7  :4;	 //Ԥ��_7
		unsigned	Reserved_8  :8;	 //Ԥ��_8
	}Bits;
	uint8 data[8];
}CANMsg0C9Union;
extern volatile CANMsg0C9Union sCANMsg0C9;
extern volatile uint8 CANMsg0C9Status;
#define TXIFID_0C9   IDIF_TX1
static const uint8  CANMsg0C9ZEROInit[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//����Ϊ�����ź�

//MSG1F1 ����
typedef union 
 	struct
	{
		unsigned	SysPwrMd  :2;	 // System Power Mode 
		unsigned	Reserved_0  :2;	 //Ԥ��_0
		unsigned	SysOpnlMd  :3;	 // System Operational Mode��Ԥ����Ĭ�Ϸ�0 
		unsigned	SysVolMdV  :1;	 //  
		unsigned	Reserved_1  :8;	 //Ԥ��_1
		unsigned	Reserved_2  :8;	 //Ԥ��_2
		unsigned	Reserved_3  :8;	 //Ԥ��_3
		unsigned	SysVolV  :1;	 //  
		unsigned	Reserved_4  :7;	 //Ԥ��_4
		unsigned	SysVol  :8;	 // Battery Voltage 
		unsigned	Reserved_5  :8;	 //Ԥ��_5
		unsigned	Reserved_6  :1;	 //Ԥ��_6
		unsigned	SysVolMd  :2;	 // System Voltage Mode ��9-16V�� 
		unsigned	Reserved_7  :5;	 //Ԥ��_7
	}Bits;
	uint8 data[8];
}CANMsg1F1Union;
extern volatile CANMsg1F1Union sCANMsg1F1;
extern volatile uint8 CANMsg1F1Status;
#define R_ID_1F1   0x1F1
static const uint8  CANMsg1F1ZEROInit[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

