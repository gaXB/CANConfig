/************************�ļ�˵��*****************************************************
* Copyright (c) 2019 ,���ݹ㰲�����������޹�˾  All rights reserved.
*
* �ļ����ƣ�InterLayMatrix.c
* ����������InterLay��ı��������ļ�����Ҫ������Ŀ����
* �������� codewarrior IDE 11.1
* MCU˵������ģ��Ϊ�ϲ�Ӧ�ã���MCU �޹�
* ���ļ���PC �Զ����ɣ�PC �汾 V1.0, DBC �ļ�����  SK81_Lux_Body_EP1_HVAC_20150810_V1.30.dbc
* �������ڣ�  07/08/20 11:13:36
/**************************���ļ�¼******************************************************
 * 
* �汾��1.0
* ˵������ʼ�汾����
* ԭ���ߣ�
* ������ڣ�
*/
#include "InterLayMatrix.h"

/***  ���Ա�ṹ  ***/ 


//�������ݱ�������
volatile CANMsg223Union sCANMsg223;
volatile uint8 CANMsg223Status;

//�������ݱ�������
volatile CANMsg1F5Union sCANMsg1F5;
volatile uint8 CANMsg1F5Status;

volatile CANMsg261Union sCANMsg261;
volatile uint8 CANMsg261Status;

volatile CANMsg375Union sCANMsg375;
volatile uint8 CANMsg375Status;

