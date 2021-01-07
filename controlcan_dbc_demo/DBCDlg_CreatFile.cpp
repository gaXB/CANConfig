#include "stdafx.h"
#include "dbc_demo.h"
#include "dbc_demoDlg.h"
#include "utility.h"
#include "StdioFileEx.h"

int F_TXNUM, F_RXNUM;

int TBL_TXID[30], TBL_RXID[30];
int ID_FUNCT = 0x7df;
int ID_PHYSIC = 0x721;
int ID_NM = 0x403;
int ID_DIAG_SEND = 0x7A1;
int IsExStand;    // 1 为extern

char* Str_DeletEnter(char* c_data)
{
	char data[_DBC_COMMENT_MAX_LENGTH_] = "";

	int j =0 ;
	for (int i= 0; i<_DBC_COMMENT_MAX_LENGTH_ && c_data[i] != '\0'; i++)
	{
		if (c_data[i] != '\r' && c_data[i] != '\n')
		{
			data[j++] = c_data[i];
		}
		else{}
	}
	strcpy(c_data, data);
	return c_data;
}

void TBL_SearchMask(int* TBLDATA, int TBL_Length, int num)
{
	//a[10] = 
	//for (int i = 0; i < 10; )
}

void Cdbc_demoDlg::CreatFile_1()
{
	CStdioFileEx myFile, BaseFile, MatrixCFile, ConfigFile; 
	CFileException fileException, BasefileException, MatrixCFileException,  ConfigFileException;
	CString strName, BaseName;
	CString stf;

	strName = L"InterLayMatrix.h";
	BaseName = L"InterLayMatrix_base.h";


	int startbit = 0;
	int byte = 0;
	int maxlenth = 8;
	int cID = 0xff;
	int iDiff;
	int u8ResNum = 0;
	CString wStr;
	CString str;
	CString str_id;

	m_NMType.GetWindowTextW(str);


	if(!myFile.Open(DBCBaseFile + strName,  CFile::modeCreate|CFile::typeText|CFile::modeReadWrite),&fileException) 
	{ 
		TRACE("Can't open file %s, error = %u\n",DBCBaseFile+strName,fileException.m_cause); 
	}

	if(!BaseFile.Open(BaseName,  CFile::typeText|CFile::modeRead),&BasefileException) 
	{ 
		TRACE("Can't open file %s, error = %u\n",BaseName,BasefileException.m_cause); 
	}

	while(1)
	{
		if (BaseFile.ReadString(stf) == FALSE)
		{
			break;
		}
		else
		{
			byte++;
			
			if (byte == 9)
			{
				int a;
				a = stf.Find('\n');
				stf = stf.Left(a);
				stf += DBCname;
				stf += '\n';
			}
			else if (byte == 10)
			{
				int a;
				CTime time;
				time=CTime::GetCurrentTime();//得到当前时间
				CString s=time.Format(L"%c");//转换时间格式
				a = stf.Find('\n');
				stf = stf.Left(a);
				stf += s;
				stf += '\n';
			}
			myFile.WriteString(stf);
		}
	}
	BaseFile.Close();

	BaseName = L"InterLayMatrix_base.c";
	if(!BaseFile.Open(BaseName,  CFile::typeText|CFile::modeRead), &BasefileException) 
	{ 
		TRACE("Can't open file %s, error = %u\n",BaseName,BasefileException.m_cause); 
	}

	strName = L"InterLayMatrix.c";
	if(!MatrixCFile.Open(DBCBaseFile + strName,   CFile::modeCreate|CFile::typeText|CFile::modeReadWrite),&MatrixCFile) 
	{ 
		TRACE("Can't open file %s, error = %u\n",DBCBaseFile + strName, ConfigFileException.m_cause); 
	}
	byte = 0;
	while(1)
	{
		if (BaseFile.ReadString(stf) == FALSE)
		{
			break;
		}
		else
		{
			byte++;
			
			if (byte == 8)
			{
				int a;
				a = stf.Find('\n');
				stf = stf.Left(a);
				stf += DBCname;
				stf += '\n';
			}
			else if (byte == 9)
			{
				int a;
				CTime time;
				time=CTime::GetCurrentTime();//得到当前时间
				CString s=time.Format(L"%c");//转换时间格式
				a = stf.Find('\n');
				stf = stf.Left(a);
				stf += s;
				stf += '\n';
			}
			MatrixCFile.WriteString(stf);
		}
	}
	BaseFile.Close();


	BaseName = L"CANIDConfig_base.h";
	if(!BaseFile.Open(BaseName,  CFile::typeText|CFile::modeRead), &BasefileException) 
	{ 
		TRACE("Can't open file %s, error = %u\n",BaseName,BasefileException.m_cause); 
	}

	strName = L"CANIDConfig.h";
	if(!ConfigFile.Open(DBCBaseFile + strName,  CFile::modeCreate|CFile::typeText|CFile::modeReadWrite),&ConfigFileException) 
	{ 
		TRACE("Can't open file %s, error = %u\n",DBCBaseFile + strName, ConfigFileException.m_cause); 
	}
	byte = 0;
	while(1)
	{
		if (BaseFile.ReadString(stf) == FALSE)
		{
			break;
		}
		else
		{
			byte++;
			
			if (byte == 11)
			{
				int a;
				a = stf.Find('\n');
				stf = stf.Left(a);
				stf += DBCname;
				stf += '\n';
			}
			else if (byte == 12)
			{
				int a;
				CTime time;
				time=CTime::GetCurrentTime();//得到当前时间
				CString s=time.Format(L"%c");//转换时间格式
				a = stf.Find('\n');
				stf = stf.Left(a);
				stf += s;
				stf += '\n';
			}
			ConfigFile.WriteString(stf);
		}
	}
	BaseFile.Close();
	//写入头end
	if (ComneedSave)  
	{  
		CString strCom;  
		m_comBox.GetWindowText(strCom);  
		m_msgList.SetItemText(m_ComItem,m_ComSubItem,strCom);  
	}  
	//写入TX 的数据
	F_TXNUM = 0;
	F_RXNUM = 0;

	MatrixCFile.WriteString(L"//发送数据变量声明\n");
	for (int i = 0; i< MsgNum; i++)
	{
		startbit = 0;
		byte = 0;
		maxlenth = 8;
		cID = 0xff;
		u8ResNum = 0;

		SloveMsg(m_vMsg[i], i);
		
		if (oneWriteByte.mType == MTYPE_T)
		{
			str.Format(L"//MSG名称： %c \n", m_vMsg[oneWriteByte.MsgID].strName);
			str =L"//MSG名称:  " +  (CString)m_vMsg[oneWriteByte.MsgID].strName + L"\n";
			myFile.WriteString(str);
			str.Format(L"//MSG%X 定义\n", m_vMsg[oneWriteByte.MsgID].nID);
			myFile.WriteString(str);
			myFile.WriteString(L"typedef union{ \n \tstruct\n\t{\n");

			do
			{
				maxlenth = 8 - startbit%8;
				for (int i = 0; i< oneWriteByte.signalNum; i++)
				{
					iDiff = oneWriteByte.signal[i].start -startbit;
					if (iDiff >= 0 && iDiff < 8)
					{
						if (iDiff < maxlenth)
						{
							maxlenth = iDiff;
							cID = i;
						}
						else{}
					}
					else
					{}
				}

				if (maxlenth == 0)
				{
					wStr = _T("\t\tunsigned\t");
					str =  oneWriteByte.signal[cID].strName;
					wStr += str;
					str.Format(_T("  :%d;"), oneWriteByte.signal[cID].length);
					wStr += str;
					wStr +=_T("\t //");
					str =  Str_DeletEnter(oneWriteByte.signal[cID].strComment); ;
					wStr += str;
					wStr +=_T("\n");
					startbit += oneWriteByte.signal[cID].length;
				}
				else
				{
					wStr = _T("\t\tunsigned\t");
					str.Format(_T("Reserved_%d"), u8ResNum);
					wStr += str;
					str.Format(_T("  :%d;"), maxlenth);
					wStr += str;
					wStr +=_T("\t //");
					str.Format(_T("预留_%d"), u8ResNum++);
					wStr += str;
					wStr +=_T("\n");
					startbit += maxlenth;
				}
				myFile.WriteString(wStr);
			}while(startbit <= 63);
			myFile.WriteString(_T("\t}Bits;\n\tuint8 data[8];\n"));


			if (m_vMsg[oneWriteByte.MsgID].nExtend)
			{
				str.Format(_T("}CANMsg%08XUnion;\n"),m_vMsg[oneWriteByte.MsgID].nID);
				str_id.Format(L"%08X",m_vMsg[oneWriteByte.MsgID].nID);
			}
			else
			{
				str.Format(_T("}CANMsg%03XUnion;\n"),m_vMsg[oneWriteByte.MsgID].nID);
				str_id.Format(L"%03X",m_vMsg[oneWriteByte.MsgID].nID);
			}

			myFile.WriteString(str);
			str =L"extern volatile CANMsg" + str_id + L"Union sCANMsg" + str_id + L";\n";
			myFile.WriteString(str);
			
			str =L"volatile CANMsg" + str_id + L"Union sCANMsg" + str_id + L";\n";
			MatrixCFile.WriteString(str);

			str =L"extern volatile uint8 CANMsg" + str_id + L"Status" + L";\n";
			myFile.WriteString(str);

			str =L"volatile uint8 CANMsg" + str_id + L"Status" + L";\n\n";
			MatrixCFile.WriteString(str);

			str =L"#define TXIFID_" + str_id + L"   IDIF_TX";
			myFile.WriteString(str);
			TBL_TXID[F_TXNUM] = m_vMsg[oneWriteByte.MsgID].nID;
			F_TXNUM++;
			str.Format(L"%d\n", F_TXNUM);
			myFile.WriteString(str);
			str.Format(L"static const uint8  CANMsg" + str_id +  L"Init[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};");
			myFile.WriteString(str);
			str.Format(L"\n\n");
			myFile.WriteString(str);
		}
	}

	myFile.WriteString(L"//以下为接收信号\n\n");
	MatrixCFile.WriteString(L"//接收数据变量声明\n");
	//rec msg file
	for (int i = 0; i< MsgNum; i++)
	{
		startbit = 0;
		byte = 0;
		maxlenth = 8;
		cID = 0xff;
		u8ResNum = 0;

		SloveMsg(m_vMsg[i], i);
		
		if (oneWriteByte.mType == MTYPE_R)
		{
			str.Format(L"//MSG名称： %c \n", m_vMsg[oneWriteByte.MsgID].strName);
			str =L"//MSG名称:  " +  (CString)m_vMsg[oneWriteByte.MsgID].strName + L"\n";
			myFile.WriteString(str);
			str.Format(L"//MSG%X 定义\n", m_vMsg[oneWriteByte.MsgID].nID);
			myFile.WriteString(str);
			myFile.WriteString(L"typedef union{ \n \tstruct\n\t{\n");

			do
			{
				maxlenth = 8 - startbit%8;
				for (int i = 0; i< oneWriteByte.signalNum; i++)
				{
					iDiff = oneWriteByte.signal[i].start -startbit;
					if (iDiff >= 0 && iDiff < 8)
					{
						if (iDiff < maxlenth)
						{
							maxlenth = iDiff;
							cID = i;
						}
						else{}
					}
					else
					{}
				}

				if (maxlenth == 0)
				{
					wStr = _T("\t\tunsigned\t");
					str =  oneWriteByte.signal[cID].strName;
					wStr += str;
					str.Format(_T("  :%d;"), oneWriteByte.signal[cID].length);
					wStr += str;
					wStr +=_T("\t //");
					str =  Str_DeletEnter(oneWriteByte.signal[cID].strComment); ;
					wStr += str;
					wStr +=_T("\n");
					startbit += oneWriteByte.signal[cID].length;
				}
				else
				{
					wStr = _T("\t\tunsigned\t");
					str.Format(_T("Reserved_%d"), u8ResNum);
					wStr += str;
					str.Format(_T("  :%d;"), maxlenth);
					wStr += str;
					wStr +=_T("\t //");
					str.Format(_T("预留_%d"), u8ResNum++);
					wStr += str;
					wStr +=_T("\n");
					startbit += maxlenth;
				}
				myFile.WriteString(wStr);
			}while(startbit <= 63);
			myFile.WriteString(_T("\t}Bits;\n\tuint8 data[8];\n"));

			if (m_vMsg[oneWriteByte.MsgID].nExtend)
			{
				str.Format(_T("}CANMsg%08XUnion;\n"),m_vMsg[oneWriteByte.MsgID].nID);
				str_id.Format(L"%08X",m_vMsg[oneWriteByte.MsgID].nID);
			}
			else
			{
				str.Format(_T("}CANMsg%03XUnion;\n"),m_vMsg[oneWriteByte.MsgID].nID);
				str_id.Format(L"%03X",m_vMsg[oneWriteByte.MsgID].nID);
			}

			myFile.WriteString(str);
			str =L"extern volatile CANMsg" + str_id + L"Union sCANMsg" + str_id + L";\n";
			myFile.WriteString(str);
			str =L"volatile CANMsg" + str_id + L"Union sCANMsg" + str_id + L";\n";
			MatrixCFile.WriteString(str);

			str =L"extern volatile uint8 CANMsg" + str_id + L"Status" + L";\n";
			myFile.WriteString(str);
			str =L"volatile uint8 CANMsg" + str_id + L"Status" + L";\n\n";
			MatrixCFile.WriteString(str);


			str =L"//#define R_ID_" + str_id+ L"   0x" + str_id+ L"\n";
			myFile.WriteString(str);
			str.Format(L"static const uint8  CANMsg" + str_id +  L"Init[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};\n");
			myFile.WriteString(str);
			TBL_RXID[F_RXNUM] = m_vMsg[oneWriteByte.MsgID].nID;
			F_RXNUM++;
			myFile.WriteString(L"\n");
		}
	}

	myFile.WriteString(L"\n\n\n//定义发送矩阵表\n");
	myFile.WriteString(L"static const CANMsgTXAttrStruct CANMsgTX_AttrTable[] =\n{\n");
	for (int i = 0; i< MsgNum; i++)
	{
		startbit = 0;
		byte = 0;
		maxlenth = 8;
		cID = 0xff;
		u8ResNum = 0;

		SloveMsg(m_vMsg[i], i);
		
		if (oneWriteByte.mType == MTYPE_T)
		{
			if (m_vMsg[oneWriteByte.MsgID].nExtend)
			{
				str_id.Format(L"%08X",m_vMsg[oneWriteByte.MsgID].nID);
				IsExStand = 1;
			}
			else
			{
				str_id.Format(L"%03X",m_vMsg[oneWriteByte.MsgID].nID);
				IsExStand = 0;
			}
			str = (L"\t{ TXIFID_") + str_id + L",  sCANMsg" + str_id + L".data,  ";
			myFile.WriteString(str);
			str.Format(L"%d,  ", (int)m_vMsg[oneWriteByte.MsgID].nCycleTime);
			myFile.WriteString(str);
			str = (L"&CANMsg") + str_id + L"Status,  " +  L"CANMsg" + str_id + L"Init},\n";
			myFile.WriteString(str);
		}
	}
	myFile.WriteString(L"};\n#define   CANMSG_TXATTR    (sizeof(CANMsgTX_AttrTable)/sizeof(CANMsgTXAttrStruct))\n\n");

	myFile.WriteString(L"\n//定义接收矩阵表\n");
	myFile.WriteString(L"static const CANMsgRXAttrStruct CANMsgRX_AttrTable[] =\n{\n");

	for (int i = 0; i< MsgNum; i++)
	{
		startbit = 0;
		byte = 0;
		maxlenth = 8;
		cID = 0xff;
		u8ResNum = 0;

		SloveMsg(m_vMsg[i], i);
		
		if (oneWriteByte.mType == MTYPE_R)
		{
			if (m_vMsg[oneWriteByte.MsgID].nExtend)
			{
				str_id.Format(L"%08X",m_vMsg[oneWriteByte.MsgID].nID);
			}
			else
			{
				str_id.Format(L"%03X",m_vMsg[oneWriteByte.MsgID].nID);
			}
			str = (L"\t{ R_ID_") + str_id + L",  sCANMsg" + str_id + L".data,  ";
			myFile.WriteString(str);
			str.Format(L"%d,  ", (int)m_vMsg[oneWriteByte.MsgID].nCycleTime*10);
			myFile.WriteString(str);
			str = (L"&CANMsg") + str_id + L"Status,  " +  L"CANMsg" + str_id + L"Init,  CANMsgUNCTimeout, CANMsgUNCMask},\n";
			myFile.WriteString(str);
		}
	}

	myFile.WriteString(L"};\n#define   CANMSG_RXATTR    (sizeof(CANMsgRX_AttrTable)/sizeof(CANMsgRXAttrStruct))\n\n#endif\n");

	//写CANIDConfig.h
	for (int i = 0; i< MsgNum; i++)
	{
		SloveMsg(m_vMsg[i], i);
		if (oneWriteByte.mType == MTYPE_DIAG_CLM)
		{
			ID_DIAG_SEND = m_vMsg[i].nID;
		}
		else if (oneWriteByte.mType == MTYPE_DIAG_FUN)
		{
			ID_FUNCT = m_vMsg[i].nID;
		}
		else if (oneWriteByte.mType == MTYPE_DIAG_PHYSIC)
		{
			ID_PHYSIC = m_vMsg[i].nID;
		}
		else if (oneWriteByte.mType == MTYPE_NM_SEND)
		{
			ID_NM = m_vMsg[i].nID;
		}
	}

	ConfigFile.WriteString(L"\n#define   CAN_STANDARD_    ");
	
	if (IsExStand)
	{
		ConfigFile.WriteString(L"0   //扩展帧\n\n ");
	}
	else
	{
		ConfigFile.WriteString(L"1   //标准帧\n\n ");
	}


	ConfigFile.WriteString(L"//CAN 接收的信号定义     \n");
	for (int k = 0; k < F_RXNUM; k++)
	{
		if (IsExStand)
		{
			str_id.Format(L"%08X" ,TBL_RXID[k]);
		}
		else
		{
			str_id.Format(L"%03X" ,TBL_RXID[k]);
		}
		str = L"#define     R_ID_" + str_id + L"     0x"+str_id + L"\n";
		ConfigFile.WriteString(str);
	}

	if (IsExStand)
	{
		str_id.Format(L"%08X" , ID_FUNCT);
	}
	else
	{
		str_id.Format(L"%03X" , ID_FUNCT);
	}
	str = L"#define     ID_DIAG_FUNCT   0x" + str_id + L"\n";
	ConfigFile.WriteString(str);

	if (IsExStand)
	{
		str_id.Format(L"%08X" , ID_PHYSIC);
	}
	else
	{
		str_id.Format(L"%03X" , ID_PHYSIC);
	}
	str = L"#define     ID_DIAG_PHYSIC   0x" + str_id + L"\n\n";
	ConfigFile.WriteString(str);

	ConfigFile.WriteString(L"#if   (CAN_STANDARD_ ==1)\n#define  ID_DIAG_FUNCT_IF           ((uint8)(ID_DIAG_FUNCT&0xff))\n#define  ID_DIAG_PHYSIC_IF           ((uint8)(ID_DIAG_PHYSIC&0xff))\n");
	ConfigFile.WriteString(L"#else\n");
	ConfigFile.WriteString(L"#define  ID_DIAG_FUNCT_IF            2\n");
	ConfigFile.WriteString(L"#define  ID_DIAG_PHYSIC_IF           1\n");
	ConfigFile.WriteString(L"#endif\n\n");

	ConfigFile.WriteString(L"//CAN 接收掩码定义， 初始化函数会调用,扩展帧只有2个标准帧有4个\n");
	int IDMaskChanle = 0;
	int IDMaskRes;
	m_NMType.GetWindowTextW(str);

	CString strRID;
	int ch =0;

	if (str == L"AUTOSAR" || str == L"OSEK" )
	{//有网络管理单独使用此1个通道来滤波
		ConfigFile.WriteString(L"//网络管理报文接收单独使用一个滤波通道\n");
		str.Format(L"#define     RECEIVE_ID%d_1        0x%x\n", IDMaskChanle+1, ID_NM&0xffffff00);
		ConfigFile.WriteString(str);
		str.Format(L"#define     RECEIVE_ID%d_2        0x%x\n", IDMaskChanle+1, (ID_NM&0xffffff00) + 0xff);
		ConfigFile.WriteString(str);
		ConfigFile.WriteString(L"#define  RECEIVE_ID1      (RECEIVE_ID1_1 & RECEIVE_ID1_2)\n");
		ConfigFile.WriteString(L"#define  RECEIVE_ID1_MASK_OR  (RECEIVE_ID1_1 | RECEIVE_ID1_2)\n");

		IDMaskChanle++;
	}

	if (IsExStand)
	{
		IDMaskRes = 2- IDMaskChanle;
	}
	else
	{
		IDMaskRes = 4 - IDMaskChanle;
	}
	
	int aNum[4], rNum;

	TBL_RXID[F_RXNUM++] = ID_PHYSIC;
	TBL_RXID[F_RXNUM++] = ID_FUNCT;

	rNum = F_RXNUM;
	aNum[0] = rNum / IDMaskRes;

	aNum[1] = aNum[0];
	aNum[2] = aNum[0];
	aNum[3] = aNum[0];
	if (IsExStand == 1)
	{//extern 时 2,3 为0
		aNum[2] = 0;
		aNum[3] = 0;
	}
	else{}

	for (int i = IDMaskChanle; i <(rNum % IDMaskRes +IDMaskChanle); i++)
	{
		aNum[i]++;
	}
		
	int checktotal = 0;
	for (int i = IDMaskChanle; i < IDMaskRes+IDMaskChanle; i++)
	{
		checktotal += aNum[i];
	}

	if (checktotal != F_RXNUM)
	{
		AfxMessageBox(L"校验错误1");
	}

	rNum = IDMaskRes;
	int RxChNum = 0;
	int FixCh_Num;
	for (int i = IDMaskChanle; i < 4; i++)
	{
		str.Format(L"//写入通道%d 的掩码值\n", i+1);
		ConfigFile.WriteString(str);
		FixCh_Num = 0;
		do
		{
			if (IsExStand)
			{
				str_id.Format(L"%08X" , TBL_RXID[RxChNum]);
			}
			else
			{
				str_id.Format(L"%03X" , TBL_RXID[RxChNum]);
			}
			
			str.Format(L"#define   RECEIVE_ID%d_%d        0x", i+1, FixCh_Num+1);
			FixCh_Num++;
			ConfigFile.WriteString(str + str_id + L"\n");
			if (FixCh_Num >= aNum[i])
			{
				if (FixCh_Num >= 4)
				{
					RxChNum++;
					break;
				}
				else
				{//没有到4，继续写用上次的ID
				}
			}
			else
			{
				RxChNum++;//写下一个ID
			}
		}while(1);

		CString PrefixName;

		PrefixName.Format(L"RECEIVE_ID%d",i+1);
		ConfigFile.WriteString(L"\n\n#define  " + PrefixName + L"     (");
		
		ConfigFile.WriteString(PrefixName+ L"_1");
		for (int j= 1; j < 4 || j < aNum[i]; j++)
		{
			ConfigFile.WriteString(L" & ");
			ConfigFile.WriteString(PrefixName);
			str.Format(L"_%d", j+1);
			ConfigFile.WriteString(str);
		}
		ConfigFile.WriteString(L") \n");

		
		ConfigFile.WriteString(L"#define  " + PrefixName + L"_OR   (");
		
		ConfigFile.WriteString(PrefixName+ L"_1");
		for (int j= 1; j < 4 || j < aNum[i]; j++)
		{
			ConfigFile.WriteString(L" | ");
			ConfigFile.WriteString(PrefixName);
			str.Format(L"_%d", j+1);
			ConfigFile.WriteString(str);
		}
		ConfigFile.WriteString(L") \n");

		ConfigFile.WriteString(L"#define  " + PrefixName + L"_MASK   (" + PrefixName + L" ^ " + PrefixName + L"_OR)\n\n");
	}

	
	//CAN 发送的信号定义
	ConfigFile.WriteString(L"//CAN 发送的信号定义\n");
	ConfigFile.WriteString(L"#define   ID_DIAG_CLM    0x");
	if (IsExStand)
	{
		str.Format(L"%08X       //诊断ID" , ID_DIAG_SEND);
	}
	else
	{
		str.Format(L"%03X       //诊断ID" , ID_DIAG_SEND);
	}
	ConfigFile.WriteString(str+L"\n");

	for (int i= 0; i < F_TXNUM; i++)
	{
		if (IsExStand)
		{
			str_id.Format(L"%08X" , TBL_TXID[i]);
		}
		else
		{
			str_id.Format(L"%03X" , TBL_TXID[i]);
		}
		
		str.Format(L"#define   ID_TX%d    	   0x" + str_id + L"\n", i+1);
		ConfigFile.WriteString(str);
	}

	//CAN 发送的信号 CANIF 的逻辑号定义
	ConfigFile.WriteString(L"\n//CAN 发送的信号 CANIF 的逻辑号定义\n");
	for (int i= 0; i < F_TXNUM; i++)
	{
		if (IsExStand)
		{
			str_id.Format(L"%08X       //诊断ID" , TBL_TXID[i]);
		}
		else
		{
			str_id.Format(L"%03X       //诊断ID" , TBL_TXID[i]);
		}
		
		str.Format(L"#define   IDIF_TX%d      %d\n",i+1, i+2);
		ConfigFile.WriteString(str);
	}

	//CAN 网络管理使用说明  
	ConfigFile.WriteString(L"\n//CAN 网络管理使用说明  \n");
	
	ConfigFile.WriteString(L"#define  _NEED_OSKNM_    ");
	
	
	m_NMType.GetWindowTextW(str);
	int nmtype;
	if (str == L"AUTOSAR")
	{
		nmtype = 1;
	}
	else if (str == L"OSEK")
	{
		nmtype = 2;
	}
	else
	{
		nmtype = 0;
	}
	if (nmtype == 2)
	{
		ConfigFile.WriteString(L"1\n");
	}
	else
	{
		ConfigFile.WriteString(L"0\n");
	}
		
	ConfigFile.WriteString(L"#define  _NEED_AUTOSARNM_   ");

	if (nmtype == 1)
	{
		ConfigFile.WriteString(L"1\n");
	}
	else
	{
		ConfigFile.WriteString(L"0\n");
	}

	ConfigFile.WriteString(L"#define  _INDIRECT_NM_      ");
	if (nmtype == 0)
	{
		ConfigFile.WriteString(L"1\n");
	}
	else
	{
		ConfigFile.WriteString(L"0\n");
	}

	ConfigFile.WriteString(L"#if      _NEED_OSKNM_ || _NEED_AUTOSARNM_\n");
	if (IsExStand)
	{
		str_id.Format(L"%08X" , ID_NM);
	}
	else
	{
		str_id.Format(L"%03X" , ID_NM);
	}
	ConfigFile.WriteString(L"#define   CANID_NETMANGE   0x" + str_id + L"\n");
	
	if (IsExStand)
	{
		str_id.Format(L"%08X" , ID_NM &0xffffff00);
	}
	else
	{
		str_id.Format(L"%03X" , ID_NM &0xffffff00);
	}
	ConfigFile.WriteString(L"#define   ID_R_NM_BIEGIN   0x" + str_id + L"\n");

	if (IsExStand)
	{
		str_id.Format(L"%08X" , (ID_NM &0xffffff00) + 0xff);
	}
	else
	{
		str_id.Format(L"%03X" , (ID_NM &0xffffff00) + 0xff);
	}
	ConfigFile.WriteString(L"#define  ID_R_NM_END  0x" + str_id + L"\n");
	ConfigFile.WriteString(L"#else\n");
	if (IsExStand)
	{
		str_id.Format(L"%08X" , ID_NM);
	}
	else
	{
		str_id.Format(L"%03X" , ID_NM);
	}
	ConfigFile.WriteString(L"#define  CANID_NETMANGE  0x" + str_id + L"\n");

	ConfigFile.WriteString(L"#endif\n\n");

	// CAN IF 层的逻辑号数量及定义
	ConfigFile.WriteString(L"// CAN IF 层的逻辑号数量及定义\n");
	str.Format(L"#define MAX_SEND_NUM  %d\n", F_TXNUM+2);
	ConfigFile.WriteString(str);
	ConfigFile.WriteString(L"static const uint32 TBL_SENDID[MAX_SEND_NUM] ={CANID_NETMANGE, ID_DIAG_CLM");
	for (int i = 0; i< F_TXNUM; i++)
	{
		str.Format(L", ID_TX%d", i+1);
		ConfigFile.WriteString(str);
	}
	ConfigFile.WriteString(L"};\n\n#endif /* CANIDCONFIG_H_ */\n");
}