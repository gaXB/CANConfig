/************************�ļ�˵��*****************************************************
* Copyright (c) 2019 ,���ݹ㰲�����������޹�˾  All rights reserved.
*
* �ļ����ƣ�InterLayMatrix.h
* ����������InterLay�����ݷ��ͣ�Ĭ��ֵ��timeoutֵ�����ͽ��ձ�Ķ��塣�밴�淶��д
* �������� codewarrior IDE 11.1 
* MCU˵������ģ��Ϊ�ϲ�Ӧ�ã���MCU �޹�
* 
*���ļ���PC �Զ����ɣ�PC �汾 V1.0, DBC �ļ�����  
* �������ڣ�  
*/
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
