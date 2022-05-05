
// dbc_demoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "dbc_demo.h"
#include "dbc_demoDlg.h"
#include "utility.h"
#include "StdioFileEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define MTYPE_MAXNUM    8
CString abc[MTYPE_MAXNUM] = {L"NC", L"T", L"R",L"DIAG_CLM",L"DIAG_FUNCT", L"DIAG_PHYSIC", L"NM_REC",L"NM_SEND"};

MSG_TYPE GetType(CString lk_str)
{
	int i;
	for (i = 0; i < MTYPE_MAXNUM; i++)
	{
		if (abc[i] == lk_str)
		{
			return (MSG_TYPE)i;
		}
	}

	return MTYPE_NC;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Cdbc_demoDlg 对话框

// 处理帧发送
static bool __stdcall OnSendFunc(void* ctx, void* pObj)
{
	Ctx* info = static_cast<Ctx*>(ctx);
	VCI_CAN_OBJ* obj = static_cast<VCI_CAN_OBJ*>(pObj);
	if (VCI_Transmit(info->devinfo.nDevType, info->devinfo.nDevIndex, info->devinfo.nChIndex, obj, 1))
	{
		Cdbc_demoDlg* pdlg = static_cast<Cdbc_demoDlg*>(info->owner);
		if (pdlg)
		{
			pdlg->AddObjToList(*obj, 0);
		}
		return true;
	}
	return false;
}

// 多帧处理完会进入该函数
static void __stdcall OnMultiTransDoneFunc(void* ctx, DBCMessage* pMsg, uint8* data, uint16 nLen, uint8 nDirection)
{
	Ctx* info = static_cast<Ctx*>(ctx);
	if (info)
	{
		Cdbc_demoDlg* pdlg = static_cast<Cdbc_demoDlg*>(info->owner);
		if (pdlg)
		{
			pdlg->AddObjToList(*pMsg, data, nLen, nDirection);
		}
	}
}

Cdbc_demoDlg::Cdbc_demoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cdbc_demoDlg::IDD, pParent)
	, m_nSglValue(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);//(IDR_MAINFRAME);
	m_hDBC = INVALID_DBC_HANDLE;
	m_devInfo.nDevType = VCI_USBCAN_E_U;//根据实际设备自行修改
	m_devInfo.nDevIndex = 0;
	m_devInfo.nChIndex = 0;
	m_nLastSglIndex = -1;
	m_nCurMsgIndex = -1;
	m_bClosed = FALSE;
}

void Cdbc_demoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, m_msgList);
	DDX_Control(pDX, IDC_LIST_FRM_INFO, m_frmList);
	DDX_Control(pDX, IDC_LIST_SIGNAL, m_sglList);
	DDX_Text(pDX, IDC_EDIT_SGL_VALUE, m_nSglValue);
	DDX_Control(pDX, IDC_COMBO1, m_NMType);
}

BEGIN_MESSAGE_MAP(Cdbc_demoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOAD_DBC, &Cdbc_demoDlg::OnBnClickedLoadDbc)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &Cdbc_demoDlg::OnBnClickedButtonSend)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_FRM_INFO, &Cdbc_demoDlg::OnNMClickListFrmInfo)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MSG, &Cdbc_demoDlg::OnNMClickListMsg)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SIGNAL, &Cdbc_demoDlg::OnNMClickListSignal)
	ON_BN_CLICKED(IDC_MODIFY_SGL, &Cdbc_demoDlg::OnBnClickedModifySgl)
END_MESSAGE_MAP()


// Cdbc_demoDlg 消息处理程序

BOOL Cdbc_demoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_hDBC = DBC_Init();
	if (INVALID_DBC_HANDLE == m_hDBC)
	{
		MessageBox(_T("生成DBC句柄失败!"));
		return TRUE;
	}
	m_ctx.owner = this;
	m_ctx.devinfo = m_devInfo;
	DBC_SetSender(m_hDBC, OnSendFunc, &m_ctx);
	DBC_SetOnMultiTransDoneFunc(m_hDBC, OnMultiTransDoneFunc, &m_ctx);
	InitList();

	m_NMType.AddString(_T("AUTOSAR"));  
	m_NMType.AddString(_T("INDIRECT")); 
	m_NMType.AddString(_T("OSEK")); 
	if (!InitDevice())
	{	
		//MessageBox(_T("打开设备失败!"));
		return TRUE;
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cdbc_demoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cdbc_demoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cdbc_demoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void Cdbc_demoDlg::OnBnClickedLoadDbc()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, _T("DBC Files(*.dbc)")
		, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, _T("DBC Files(*.dbc)|*.dbc||")
		, NULL);
	dlg.m_ofn.lpstrTitle = _T("选择dbc文件");
	if (IDOK == dlg.DoModal())
	{
		string strFile = zy::W2AEx(dlg.GetPathName());
	
		if (strFile.empty())
		{

			MessageBox(_T("路径为空!"));
			return;
		}	
		
		CString stf;
		int nIndex = 0;
		stf = strFile.c_str();

		int findIndex = 0;
		do{
			nIndex = findIndex;
			findIndex = stf.Find(_T("\\"), nIndex+1);
		}while(findIndex != -1);
		
		/*do
		{
			nIndex = stf.Find(_T("\\"));
			if(nIndex ==-1) break;
			stf =stf.Mid(nIndex+1);
		}while (nIndex !=-1);*/

		DBCname = stf.Mid(nIndex+1);
		DBCBaseFile = stf.Left(nIndex+1);
		FileInfo fileinfo;
		strcpy_s(fileinfo.strFilePath, _MAX_FILE_PATH_, strFile.c_str());
		fileinfo.type = PROTOCOL_OTHER;
		fileinfo.merge = false;
		if (!DBC_LoadFile(m_hDBC, &fileinfo))
		{
			MessageBox(_T("加载文件失败!"));
			return;
		}
		if (0 == DBC_GetMessageCount(m_hDBC))
		{
			MessageBox(_T("文件中不含有消息!"));
			return;
		}
		ReadAllMessage();
		m_nCurMsgIndex = -1;
	}
}

void Cdbc_demoDlg::OnBnClickedButtonSend()
{
	// TODO: Add your control notification handler code here
/*	if (m_nCurMsgIndex >= 0)
	{
		DBCMessage& msg = m_vMsg[m_nCurMsgIndex];
		DBC_Send(m_hDBC, &msg);
	}*/

	CreatFile_1();
}

void Cdbc_demoDlg::InitList()
{
	DWORD nStyle;
	m_msgList.InsertColumn(0, _T("消息名"), LVCFMT_LEFT, 120);
	m_msgList.InsertColumn(1, _T("ID"), LVCFMT_LEFT, 100);
	m_msgList.InsertColumn(2, _T("T/R"), LVCFMT_LEFT, 100);
	nStyle = m_msgList.GetExtendedStyle();
	nStyle |= LVS_EX_FULLROWSELECT ;
	nStyle |= LVS_EX_GRIDLINES;
	m_msgList.SetExtendedStyle(nStyle);

	m_frmList.InsertColumn(0, _T("方向"), LVCFMT_LEFT, 40);
	m_frmList.InsertColumn(1, _T("ID"), LVCFMT_LEFT, 100);
	m_frmList.InsertColumn(2, _T("帧类型"), LVCFMT_LEFT, 70);
	m_frmList.InsertColumn(3, _T("长度"), LVCFMT_LEFT, 50);
	m_frmList.InsertColumn(4, _T("数据"), LVCFMT_LEFT, 200);
	nStyle = m_frmList.GetExtendedStyle();
	nStyle |= LVS_EX_FULLROWSELECT;
	m_frmList.SetExtendedStyle(nStyle);

	m_sglList.InsertColumn(0, _T("信号名"), LVCFMT_LEFT, 120);
	m_sglList.InsertColumn(1, _T("值"), LVCFMT_LEFT, 70);
	m_sglList.InsertColumn(2, _T("注释"), LVCFMT_LEFT, 140);
	nStyle = m_sglList.GetExtendedStyle();
	nStyle |= LVS_EX_FULLROWSELECT;
	m_sglList.SetExtendedStyle(nStyle);

	rowCount = m_msgList.GetItemCount();  //获取行数
	needSave = false; //初始化为FLASE;‘
	ComneedSave = false;
}

void Cdbc_demoDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_bClosed = TRUE;
	DBC_Release(m_hDBC);
	VCI_CloseDevice(m_devInfo.nDevType, m_devInfo.nDevIndex);
	CDialog::OnClose();
}

void Cdbc_demoDlg::ReadAllMessage()
{
	m_sglList.DeleteAllItems();
	m_msgList.DeleteAllItems();
	m_vMsg.clear();
	DBCMessage msg;
	if (DBC_GetFirstMessage(m_hDBC, &msg))
	{
		AddMsgToList(msg, &m_msgList);
		MsgNum = 1;
		while(DBC_GetNextMessage(m_hDBC, &msg))
		{
			AddMsgToList(msg, &m_msgList);
			MsgNum++;
		}
	}
}

void Cdbc_demoDlg::AddMsgToList(const DBCMessage& msg, CListCtrl* pList)
{
	int nCount = pList->GetItemCount();
	int nRow = pList->InsertItem(nCount, CString(msg.strName));
	CString str;
	str.Format(_T("0x%08X"), msg.nID);
	pList->SetItemText(nRow, 1, str);
	m_vMsg.push_back(msg);
	TRACE("%08X\n", msg.nID);
	for (int i = 0; i < msg.nSignalCount; ++i)
	{
		const DBCSignal& signal = msg.vSignals[i];
		TRACE("is motorola:%d\n", signal.is_motorola);
	}
}

void Cdbc_demoDlg::AddObjToList(const DBCMessage& msg, uint8* data, uint16 nLen, uint8 nDirection )
{
	int nCount = m_frmList.GetItemCount();
	int nRow = m_frmList.InsertItem(nCount, nDirection?_T("接收"):_T("发送"));
	CString str, strTmp;
	str.Format(_T("0x%08X"), msg.nID);
	m_frmList.SetItemText(nRow, 1, str);
	m_frmList.SetItemText(nRow, 2, CString(msg.strName));
	str.Format(_T("%02d"), nLen);
	m_frmList.SetItemText(nRow, 3, str);
	str = _T("");
	for (int i = 0; i < nLen; ++i)
	{
		strTmp.Format(_T("%02X "), data[i]);
		str += strTmp;
	}
	m_frmList.SetItemText(nRow, 4, str);
	m_vMultiMsg[nCount] = msg;
}

void Cdbc_demoDlg::AddObjToList( const VCI_CAN_OBJ& obj, CListCtrl* pList, byte nDirection )
{
	int nCount = pList->GetItemCount();
	int nRow = pList->InsertItem(nCount, nDirection?_T("接收"):_T("发送"));
	CString str;
	str.Format(_T("0x%08X"), obj.ID);
	pList->SetItemText(nRow, 1, str);
	pList->SetItemText(nRow, 2, obj.ExternFlag?_T("扩展帧"):_T("标准帧"));
	str.Format(_T("%02d"), obj.DataLen);
	pList->SetItemText(nRow, 3, str);
	CString strTmp;
	str = _T("");
	for (int i = 0; i < obj.DataLen; ++i)
	{
		strTmp.Format(_T("%02X "), obj.Data[i]);
		str += strTmp;
	}
	pList->SetItemText(nRow, 4, str);
	m_frmBuffer.AddObj((VCI_CAN_OBJ*)&obj, 1);
}

void Cdbc_demoDlg::AddObjToList( const VCI_CAN_OBJ& obj, byte nDirection)
{
	AddObjToList(obj, &m_frmList, nDirection);
}

void Cdbc_demoDlg::AddSglToList( const DBCMessage& msg, CListCtrl* pList )
{
	uint32 nCount = msg.nSignalCount;
	pList->DeleteAllItems();
	for (uint32 i = 0; i < nCount; ++i)
	{
		AddSglToList(msg.vSignals[i], pList);
	}
}




void Cdbc_demoDlg::SloveMsg( const DBCMessage& msg, int mID)
{
	uint32 nCount = msg.nSignalCount;
	uint32 nByte = 0;
	int start, end;

	int length, sign_length, start_length;
	char sign_suffix[4]="";
	char commment_suffix[5]="";
	bool bextern_byte;
	sign_suffix[0] = '_';
	commment_suffix[0]= '<';
	commment_suffix[1]= '<';
	oneWriteByte.signalNum = 0;
	CString str;

	str = m_msgList.GetItemText(mID,2);
	oneWriteByte.mType = GetType(str);

	for (uint32 i =0; i < msg.nSignalCount && i < 64; ++i)
	{
		sign_suffix[1] = '1';
		bextern_byte = false;
					
		start_length =  msg.vSignals[i].nStartBit;
		sign_length = msg.vSignals[i].nLen;
		if (msg.vSignals[i].is_motorola)
		{
			do
			{
				length = start_length%8+1;
				
				if (sign_length <= length)
				{
					oneWriteByte.signal[oneWriteByte.signalNum].length = sign_length;
					
					sign_length = 0;
				}
				else
				{
					oneWriteByte.signal[oneWriteByte.signalNum].length = length;
					
					sign_length = sign_length - length;
					bextern_byte = true;
				}

				oneWriteByte.signal[oneWriteByte.signalNum].start = start_length+1 - oneWriteByte.signal[oneWriteByte.signalNum].length;

				start_length = start_length + 16 - oneWriteByte.signal[oneWriteByte.signalNum].length;
				
				strcpy(oneWriteByte.signal[oneWriteByte.signalNum].strName, msg.vSignals[i].strName);
				strcpy(oneWriteByte.signal[oneWriteByte.signalNum].strComment,msg.vSignals[i].strComment);
				if (bextern_byte)
				{
					strcat(oneWriteByte.signal[oneWriteByte.signalNum].strName, sign_suffix);  
					if (sign_length >= 10)
					{
						commment_suffix[2] = '0'+ sign_length/10;
						commment_suffix[3] = '0'+ sign_length%10;
					}
					else
					{
						commment_suffix[2] = '0'+ sign_length;
					}
					strcat(oneWriteByte.signal[oneWriteByte.signalNum].strComment, commment_suffix);   
				}
				oneWriteByte.signalNum++;
				sign_suffix[1]++;
				
			}while(sign_length > 0);
		}
		else
		{//intel
			oneWriteByte.signal[oneWriteByte.signalNum].length = sign_length;
			oneWriteByte.signal[oneWriteByte.signalNum].start = start_length;
			strcpy(oneWriteByte.signal[oneWriteByte.signalNum].strName, msg.vSignals[i].strName);
			strcpy(oneWriteByte.signal[oneWriteByte.signalNum].strComment, msg.vSignals[i].strComment);
			oneWriteByte.signalNum++;
		}
	}

	oneWriteByte.MsgID = mID;
}

void Cdbc_demoDlg::AddSglToList( const DBCSignal& sgl, CListCtrl* pList )
{
	int nCount = pList->GetItemCount();
	pList->InsertItem(nCount, CString(sgl.strName));
	CString str;
	CString strUnit;
	strUnit = sgl.unit;
	str.Format(_T("%.16g%s"), sgl.nValue, strUnit);
	pList->SetItemText(nCount, 1, str);
	pList->SetItemText(nCount, 2, CString(sgl.strComment));
}

//1000Kbps 800Kbps 500Kbps 250Kbps 125Kbps 100Kbps 50Kbps 20Kbps 10Kbps 5Kbps
static const UCHAR g_timing0[] = {0x00, 0x00, 0x00, 0x01, 0x03, 0x04, 0x09, 0x18, 0x31, 0xBF};
static const UCHAR g_timing1[] = {0x14, 0x16, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0xFF};
static const DWORD g_baudrates[] = {0x060003, 0x060004, 0x060007,0x1C0008, 0x1C0011, 0x160023,
	0x1C002C, 0x1600B3, 0x1C00E0, 0x1C01C1};

BOOL Cdbc_demoDlg::InitDevice()
{
	if (VCI_OpenDevice(m_devInfo.nDevType, m_devInfo.nDevIndex, 0) != STATUS_OK)
	{
		return FALSE;
	}
	VCI_INIT_CONFIG cfg;
	cfg.Mode = 0;
	BOOL bCanCardU = IsCanCardU(m_devInfo.nDevType);
	if (bCanCardU)
	{
		//设置波特率 1000kbps
		DWORD nBaud = g_baudrates[0];
		if (VCI_SetReference(m_devInfo.nDevType, m_devInfo.nDevIndex, m_devInfo.nChIndex, 0, &nBaud) != STATUS_OK)
		{
			return FALSE;
		}
	}
	else
	{
		//设置波特率 1000kbps
		cfg.Filter = 0;
		cfg.AccCode = 0;
		cfg.AccMask = 0xFFFFFFFF;
		cfg.Timing0 = g_timing0[0];
		cfg.Timing1 = g_timing1[0];
	}
	if (VCI_InitCAN(m_devInfo.nDevType, m_devInfo.nDevIndex, m_devInfo.nChIndex, &cfg) != STATUS_OK)
	{
		return FALSE;
	}
	if (VCI_StartCAN(m_devInfo.nDevType, m_devInfo.nDevIndex, m_devInfo.nChIndex) != STATUS_OK)
	{
		return FALSE;
	}
	_beginthreadex(NULL, 0, OnReceive, this, 0, NULL);
	return TRUE;
}

BOOL Cdbc_demoDlg::IsCanCardU( DWORD nType )
{
	static DWORD vDevTypes[] = {VCI_USBCAN_8E_U, VCI_USBCAN_4E_U, VCI_PCI5020U, VCI_USBCAN_2E_U
	, VCI_USBCAN_E_U, VCI_PCI5010U};
	for (int i = 0; i < _countof(vDevTypes); ++i)
	{
		if (nType == vDevTypes[i])
		{
			return TRUE;
		}
	}
	return FALSE;
}

unsigned WINAPI Cdbc_demoDlg::OnReceive( void* data )
{
	Sleep(1000);
	Cdbc_demoDlg* pDlg = static_cast<Cdbc_demoDlg*>(data);
	if (pDlg)
	{
		pDlg->ReceviveFrm();
	}
	return 0;
}

void Cdbc_demoDlg::ReceviveFrm()
{
	VCI_CAN_OBJ vObjs[100];
	DWORD nCountRev;
	while(!m_bClosed)
	{
		nCountRev = VCI_GetReceiveNum(m_devInfo.nDevType, m_devInfo.nDevIndex, m_devInfo.nChIndex);
		if (0 == nCountRev)
		{
			Sleep(1);
			continue;
		}
		nCountRev = nCountRev > 100 ? 100 : nCountRev;
		nCountRev = VCI_Receive(m_devInfo.nDevType, m_devInfo.nDevIndex, m_devInfo.nChIndex, vObjs, nCountRev, 1);
		for (DWORD i = 0; i < nCountRev; ++i)
		{
			AddObjToList(vObjs[i], &m_frmList, 1);
			if (m_hDBC != INVALID_DBC_HANDLE)
			{
				DBC_OnReceive(m_hDBC, &vObjs[i]);
			}
		}
	}
}

void Cdbc_demoDlg::OnNMClickListFrmInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pList = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	if (pList->iItem != -1)
	{
		m_nCurObjIndex = pList->iItem;
		std::map<int, DBCMessage>::iterator it;
		if ((it = m_vMultiMsg.find(m_nCurObjIndex)) != m_vMultiMsg.end())
		{   //是多帧消息
			SetMsgCurSelectedItem(it->second);
			AddSglToList(it->second, &m_sglList);
			m_nLastSglIndex = -1;
		}
		else
		{
			VCI_CAN_OBJ obj;
			if (m_frmBuffer.CopySingleObjbyPos(obj, m_nCurObjIndex))
			{
				m_sglList.DeleteAllItems();
				DBCMessage msg;
				if (DBC_Analyse(m_hDBC, &obj, &msg))
				{
					SetMsgCurSelectedItem(msg);
					AddSglToList(msg, &m_sglList);
					m_nLastSglIndex = -1;
				}
			}
		}
	}
	*pResult = 0;
}

void Cdbc_demoDlg::OnNMClickListMsg(NMHDR *pNMHDR, LRESULT *pResult)
{
/*	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pList = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	if (pList->iItem != -1)
	{
		m_frmList.SetItemState(m_nCurObjIndex, 0, LVIS_SELECTED|LVIS_FOCUSED);
		m_nCurMsgIndex = pList->iItem;
		AddSglToList(m_vMsg[m_nCurMsgIndex], &m_sglList);
		m_nLastSglIndex = -1;
	}
	*pResult = 0;*/


/*	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
    
          //我这里有两种办法可以实现

	NM_LISTVIEW  *pEditCtrl = (NM_LISTVIEW *)pNMHDR;
	CRect  EditRect;
	//CPoint  point;
	//GetCursorPos(&point);
	//m_list.ScreenToClient(&point);
    
	//LVHITTESTINFO info;
	//info.pt = point;
	//info.flags = LVHT_ABOVE;
    
    if (needSave)
    {  
	   //needSave == TURE
	   CString  EditStr;
	   m_Edit.GetWindowText(EditStr); //获取输入的内容
	   m_msgList.SetItemText(m_Item,m_SubItem,EditStr); //m_Item,m_SubItem这两个变量的值是原来的单元格的值

    }


	m_Item = pEditCtrl->iItem;
         m_SubItem = pEditCtrl->iSubItem;
       //m_Item = m_list.SubItemHitTest(&info);

	if (m_Item == -1)
	{
		return;
	}
      //m_SubItem = info.iSubItem;

	//if (m_Item < rowCount)
	if (1)
	{

		m_msgList.GetSubItemRect(m_Item,m_SubItem,LVIR_LABEL,EditRect);
		needSave = true;
		if (m_Edit.m_hWnd == NULL)

		{    //m_Edit.m_hWnd == NULL说明还没创建
			//RECT rect;
			//rect.left = rect.top = 0;
			//rect.bottom = 20;
			//rect.right = 100;

			m_Edit.Create(ES_AUTOHSCROLL|WS_CHILD|ES_LEFT|ES_WANTRETURN|WS_BORDER,
						CRect(0,0,0,0),this,IDC_EDIT);

			//IDC_EDIT已近在头文件中定义,这个很重要,很多时候会忽略,
			//网上找到的实现编辑办法中这个都没说明
			//我定义为#define  IDC_EDIT 0xffff

			m_Edit.ShowWindow(SW_HIDE); //Edit创建完后隐藏
			m_Edit.SetFont(this->GetFont(),FALSE);//设置字体
			/*
			CFont tpFont;
			tpFont.CreateStockObject(DEFAULT_GUI_FONT);
			m_Edit.SetFont(&tpFont);
			tpFont.DeleteObject();
		   */
/*		}
		m_Edit.SetParent(&m_msgList); //将list control设置为父窗口,生成的Edit才能正确定位,这个也很重要,

		EditRect.SetRect(EditRect.left,EditRect.top,EditRect.left+m_msgList.GetColumnWidth(m_SubItem),EditRect.bottom);
		//m_list.GetColumnWidth(m_SubItem)获取列的宽度

		CString strItem = m_msgList.GetItemText(m_Item,m_SubItem);
		m_Edit.MoveWindow(&EditRect);
		m_Edit.ShowWindow(SW_SHOW);
		m_Edit.SetWindowText(strItem);
		m_Edit.SetFocus();//设置为焦点
		m_Edit.SetSel(0,-1);  //0,-1表示单元格内容全选中

	}
	else
	m_Edit.ShowWindow(SW_HIDE);
    *pResult = 0;*/


	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);  
    // TODO: 在此添加控件通知处理程序代码  
      
         //我这里有两种办法可以实现  
  
    NM_LISTVIEW  *pEditCtrl = (NM_LISTVIEW *)pNMHDR;  
    CRect  EditRect;  
    CRect  ComRect;  
    //CPoint  point;  
    //GetCursorPos(&point);  
    //m_list.ScreenToClient(&point);  
  
    //LVHITTESTINFO info;  
    //info.pt = point;  
    //info.flags = LVHT_ABOVE  
  
  
      
    //m_Item = m_list.SubItemHitTest(&info);  
  
  
    if (pEditCtrl->iItem == -1)  
    {   
        //当点击其他区域时,显示内容  
        if (needSave)  
        {    
            //needSave == TURE  
            CString  EditStr;  
            m_Edit.GetWindowText(EditStr); //获取输入的内容  
            m_msgList.SetItemText(m_Item,m_SubItem,EditStr); //m_Item,m_SubItem这两个变量的值是原来的单元格的值  
                                m_Edit.ShowWindow(SW_HIDE);  
        }  
       
        if (ComneedSave)  
        {  
            CString strCom;  
            m_comBox.GetWindowText(strCom);  
            m_msgList.SetItemText(m_ComItem,m_ComSubItem,strCom);  
            m_comBox.ShowWindow(SW_HIDE);  
        }  
  
        /* 
        if (m_Edit.m_hWnd != NULL) 
        { 
            m_Edit.ShowWindow(SW_HIDE); 
        } 
        if (m_comBox.m_hWnd != NULL) 
        { 
            m_comBox.ShowWindow(SW_HIDE); 
        } 
        */  
        return;  
    }  
              //m_SubItem = info.iSubItem;  
  
  
    //在1,2,4列实现编辑  
    if (pEditCtrl->iItem < rowCount && pEditCtrl->iSubItem != 2)  
    {  
        if (m_comBox.m_hWnd != NULL)  
        {  
            CString strCom;  
            m_comBox.GetWindowText(strCom);  
            m_msgList.SetItemText(m_ComItem,m_ComSubItem,strCom);//ComBoBox保存之前数据  
            m_comBox.ShowWindow(SW_HIDE);  
        }  
          
        if (needSave)  
        {    
            //needSave == TURE  
            CString  EditStr;  
            m_Edit.GetWindowText(EditStr); //获取输入的内容  
            m_msgList.SetItemText(m_Item,m_SubItem,EditStr); //m_Item,m_SubItem这两个变量的值是原来的单元格的值  
  
        }  
  
        m_Item = pEditCtrl->iItem; //行数  
        m_SubItem = pEditCtrl->iSubItem; //列数  
  
        m_msgList.GetSubItemRect(m_Item,m_SubItem,LVIR_LABEL,EditRect); //获取单元格的空间  
          
        if (m_Edit.m_hWnd == NULL)  
  
        {    //m_Edit.m_hWnd == NULL说明还没创建  
            //RECT rect;  
            //rect.left = rect.top = 0;  
            //rect.bottom = 20;  
            //rect.right = 100;  
  
                            needSave = true;  
            m_Edit.Create(ES_AUTOHSCROLL|WS_CHILD|ES_LEFT|ES_WANTRETURN|WS_BORDER,  
                        CRect(0,0,0,0),this,IDC_EDIT);  
  
            //IDC_EDIT已近在头文件中定义,这个很重要,很多时候会忽略,  
            //网上找到的实现编辑办法中这个都没说明  
            //我定义为#define  IDC_EDIT 0xffff  
  
            m_Edit.ShowWindow(SW_HIDE); //Edit创建完后隐藏  
            m_Edit.SetFont(this->GetFont(),FALSE);//设置字体  
            /* 
            CFont tpFont; 
            tpFont.CreateStockObject(DEFAULT_GUI_FONT); 
            m_Edit.SetFont(&tpFont); 
            tpFont.DeleteObject(); 
           */  
        }  
        m_Edit.SetParent(&m_msgList); //将list control设置为父窗口,生成的Edit才能正确定位,这个也很重要,  
  
        EditRect.SetRect(EditRect.left,EditRect.top,EditRect.left+m_msgList.GetColumnWidth(m_SubItem),EditRect.bottom);  
        //m_list.GetColumnWidth(m_SubItem)获取列的宽度  
  
        CString strItem = m_msgList.GetItemText(m_Item,m_SubItem);  
          
        m_Edit.MoveWindow(&EditRect);  
        m_Edit.ShowWindow(SW_SHOW);  
        m_Edit.SetWindowText(strItem);  
        m_Edit.SetFocus();//设置为焦点  
        m_Edit.SetSel(0,-1);  //0,-1表示单元格内容全选中  
          
          
  
    }  
      
             //在第三列插入Combo Box  
  
    if (pEditCtrl->iSubItem == 2)  
    {     
       if (m_Edit.m_hWnd != NULL)  
       {  
           CString  EditStr;  
           m_Edit.GetWindowText(EditStr); //获取输入的内容  
           m_msgList.SetItemText(m_Item,m_SubItem,EditStr); //m_Item,m_SubItem这两个变量的值是原来的单元格的值  
           m_Edit.ShowWindow(SW_HIDE);  
       }  
  
  
        if (ComneedSave)  
        {  
            CString strCom;  
            m_comBox.GetWindowText(strCom);  
                 m_msgList.SetItemText(m_ComItem,m_ComSubItem,strCom);  
        }  
          
        m_ComItem = pEditCtrl->iItem; //行数  
        m_ComSubItem = pEditCtrl->iSubItem; //列数  
              m_msgList.GetSubItemRect(m_ComItem,m_ComSubItem,LVIR_LABEL,ComRect);  
  
       if (m_comBox.m_hWnd == NULL)  
       {  
           ComneedSave = true;  
           m_comBox.Create(WS_CHILD|WS_VISIBLE|CBS_SORT|CBS_DROPDOWN|CBS_OEMCONVERT,CRect(0,0,40,40),this,0);  
                      //CBS_OEMCONVERT 实现可编辑  
           //CBS_DROPDOWN   产生一个下拉式框,小图标  
           //CBS_DROPDOWNLIST   图标代替单元格  
           m_comBox.AddString(_T("T"));  //添加选项  
           m_comBox.AddString(_T("R"));  
		   m_comBox.AddString(_T("DIAG_FUNCT"));  //添加选项  
           m_comBox.AddString(_T("DIAG_PHYSIC"));  
		   m_comBox.AddString(_T("DIAG_CLM"));  //添加选项  
           m_comBox.AddString(_T("NM_SEND"));  
		   m_comBox.AddString(_T("NC")); 
		   m_comBox.AddString(_T("NM_REC")); 
           m_comBox.SetFont(this->GetFont(),FALSE);  
          // m_comBox.ShowWindow(SW_HIDE);  
       }  
  
            CString  ComstrItem;  
       m_comBox.SetParent(&m_msgList);  
            ComstrItem = m_msgList.GetItemText(m_ComItem,m_ComSubItem);  
       ComRect.SetRect(ComRect.left,ComRect.top,ComRect.left+m_msgList.GetColumnWidth(m_ComSubItem),ComRect.bottom);  
       m_comBox.MoveWindow(&ComRect);  
       m_comBox.ShowWindow(SW_SHOW);  
       m_comBox.SetWindowText(ComstrItem);  //当显示Combo Box控件时里面的内容为原来的内容,不然是空白的  
              m_comBox.SetFocus(); //设置焦点  
    }  
        
	NM_LISTVIEW* pList = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	if (pList->iItem != -1)
	{
		m_frmList.SetItemState(m_nCurObjIndex, 0, LVIS_SELECTED|LVIS_FOCUSED);
		m_nCurMsgIndex = pList->iItem;
		
		AddSglToList(m_vMsg[m_nCurMsgIndex], &m_sglList);
		
		m_nLastSglIndex = -1;
	}
	*pResult = 0;  

}

void Cdbc_demoDlg::OnNMClickListSignal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pList = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	if (pList->iItem != -1)
	{
		if (m_nCurMsgIndex >= 0)
		{
			DBCSignal& sgl = m_vMsg[m_nCurMsgIndex].vSignals[pList->iItem];
			m_nSglValue = sgl.nValue;
			UpdateData(FALSE);
			m_nLastSglIndex = pList->iItem;
		}
	}
	*pResult = 0;
}

void Cdbc_demoDlg::UpdateSglValue()
{
	if (m_nLastSglIndex >= 0 && m_nCurMsgIndex >= 0)
	{
		DBCSignal& sgl = m_vMsg[m_nCurMsgIndex].vSignals[m_nLastSglIndex];
		UpdateData(TRUE);
		sgl.nValue = m_nSglValue;
		CString str, strUnit;
		strUnit = sgl.unit;
		str.Format(_T("%.16g%s"), sgl.nValue, strUnit);
		m_sglList.SetItemText(m_nLastSglIndex, 1, str);
	}
}

void Cdbc_demoDlg::SetMsgCurSelectedItem( const DBCMessage& msg )
{
	size_t nCount = m_vMsg.size();
	for (size_t i = 0; i < nCount; ++i)
	{
		if (m_vMsg[i].nID == msg.nID)
		{
			m_nCurMsgIndex = i;
			m_vMsg[i] = msg;
			m_msgList.SetItemState(m_nCurMsgIndex, LVIS_SELECTED|LVIS_FOCUSED
				, LVIS_SELECTED|LVIS_FOCUSED);
		}
	}
}

void Cdbc_demoDlg::OnBnClickedModifySgl()
{
	// TODO: Add your control notification handler code here
	UpdateSglValue();
	//m_GetStr =m_msgList.GetItemText(1,1);
}
