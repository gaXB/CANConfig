
// dbc_demoDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "IDBCManager.h"
#include "ControlCAN.h"
#include "CycleBuffer.h"
#include <vector>
#include <map>
#include "ListCtrlEx.h"
#include "afxwin.h"

struct DevInfo
{
	DWORD nDevType;
	DWORD nDevIndex;
	DWORD nChIndex; //通道号
};

struct Ctx
{
	void* owner;
	DevInfo devinfo;
};

typedef enum
{	
	MTYPE_NC = 0,          //直接跳过
	MTYPE_T, //相等
	MTYPE_R,
	MTYPE_DIAG_CLM,     //int16 /10
	MTYPE_DIAG_FUN,
	MTYPE_DIAG_PHYSIC,
	MTYPE_NM_REC,
	MTYPE_NM_SEND,
}MSG_TYPE;

typedef struct
{
	char strName[_DBC_NAME_LENGTH_+1];               //名称
    char strComment[_DBC_COMMENT_MAX_LENGTH_+1];    //注释
	int  start;
	int  length;
}ONESIGNAL;

typedef struct
{
	int signalNum;
	int MsgID;
	ONESIGNAL signal[40];
	MSG_TYPE mType;
}ONEBYTE;

#define IDC_EDIT  0x601

// Cdbc_demoDlg 对话框
class Cdbc_demoDlg : public CDialog
{
// 构造
public:
	Cdbc_demoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DBC_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedLoadDbc();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnNMClickListFrmInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListMsg(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListSignal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedModifySgl();
public:
	void ReceviveFrm();
	void AddObjToList(const VCI_CAN_OBJ& obj, byte nDirection);
	void AddObjToList(const DBCMessage& msg, uint8* data, uint16 nLen, uint8 nDirection);
private:
	void InitList();
	BOOL InitDevice();
	void ReadAllMessage();
	void AddMsgToList(const DBCMessage& msg, CListCtrl* pList);
	void AddObjToList(const VCI_CAN_OBJ& obj, CListCtrl* pList, byte nDirection);
	void AddSglToList(const DBCMessage& msg, CListCtrl* pList);
	void AddSglToList(const DBCSignal& sgl, CListCtrl* pList);
	// 是否是新型号CAN卡
	BOOL IsCanCardU(DWORD nType);
	static unsigned WINAPI OnReceive(void* data);
	void UpdateSglValue();
	void SetMsgCurSelectedItem(const DBCMessage& msg);
private:
	BOOL m_bClosed;
	CListCtrlEx m_msgList;
	CListCtrlEx m_frmList;
	CListCtrlEx m_sglList;
	DBCHandle m_hDBC;
	DevInfo m_devInfo;
	CCycleBuffer<VCI_CAN_OBJ> m_frmBuffer;//存放接收到的帧
	std::vector<DBCMessage> m_vMsg;//存放dbc文件包含的消息
	std::map<int, DBCMessage> m_vMultiMsg;//存放收发的多帧消息
	double m_nSglValue;
	int m_nCurMsgIndex;
	int m_nLastSglIndex;
	int m_nCurObjIndex;
	Ctx m_ctx;//回调上下文, 作为回调函数的参数

    CEdit m_Edit;  
    int m_Item;  //要Edit行  
    int m_SubItem; //要Edit列  
    int rowCount;  //总行数  
    int m_ComItem; // 要加入Combo Box 的行   
    int m_ComSubItem; //要加入Combo Box的列 为了区别才这样定义,下面的两个bool类型也一样  
    bool  needSave; //用于保存标志,如果有过编写则为TURE,当下次编辑时会在原来的位置输入原来输入的内容  
    bool  ComneedSave;  
    //afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);  
    CComboBox m_comBox;  
	void SloveMsg( const DBCMessage& msg, int mID);
	void CreatFile_1();
	ONEBYTE oneWriteByte;

	CString DBCname, DBCBaseFile;
	int MsgNum;
public:
	// 网络管理类型
	CComboBox m_NMType;
};
