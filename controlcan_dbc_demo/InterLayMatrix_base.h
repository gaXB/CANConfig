/************************文件说明*****************************************************
* Copyright (c) 2019 ,杭州广安汽车电器有限公司  All rights reserved.
*
* 文件名称：InterLayMatrix.h
* 功能描述：InterLay层数据发送，默认值，timeout值，发送接收表的定义。请按规范编写
* 编译器： codewarrior IDE 11.1 
* MCU说明：此模块为上层应用，和MCU 无关
* 
*此文件由PC 自动生成，PC 版本 V1.0, DBC 文件名：  
* 生成日期：  
*/
/**************************更改记录******************************************************
 *
* 版本：1.0
* 说明：初始版本建立
* 原作者：
* 完成日期：
*/
#ifndef _INTERLAY_MATRIX_H_
#define _INTERLAY_MATRIX_H_
#include "timerout.h"
#include "CANIDConfig.h"


//CAN发送数据结构体
typedef struct
{
           uint8   IfID;            //if层ID
  volatile uint8   *pMsg;        //报文名称(报文存储地址)
           uint16  Cycle;        //报文发送周期(发)
  volatile uint8   *pStatus;     //报文状态
  const    uint8   *pInit;       //报文初始化值
}CANMsgTXAttrStruct;

//CAN接收数据结构体
typedef struct
{
           uint32  ID;           //接收为报文ID
  volatile uint8   *pMsg;        //报文名称(报文存储地址)
           uint16  Cycle;        //报文超时周期(收)
  volatile uint8   *pStatus;     //报文状态
  const    uint8   *pInit;       //报文初始化值
  const    uint8   *pTimeOut;    //报文掉线值
  const    uint8   *pMask;       //报文屏蔽位(决定某一位在掉线时是否更新为掉线值)
}CANMsgRXAttrStruct;

///初始化为0，不改变的值，如果没有定义可以先用这个为默认值
static const uint8  CANMsgZEROInit[8]    = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8  CANMsgUNCTimeout[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8  CANMsgUNCMask[8]    = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const uint8  CANMsgZEROMask[8]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//
