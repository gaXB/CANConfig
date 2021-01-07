
// dbc_demoDlg.h : ͷ�ļ�
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
	DWORD nChIndex; //ͨ����
};

struct Ctx
{
	void* owner;
	DevInfo devinfo;
};

typedef enum
{	
	MTYPE_NC = 0,          //ֱ������
	MTYPE_T, //���
	MTYPE_R,
	MTYPE_DIAG_CLM,     //int16 /10
	MTYPE_DIAG_FUN,
	MTYPE_DIAG_PHYSIC,
	MTYPE_NM_REC,
	MTYPE_NM_SEND,
}MSG_TYPE;

typedef struct
{
	char strName[_DBC_NAME_LENGTH_+1];               //����
    char strComment[_DBC_COMMENT_MAX_LENGTH_+1];    //ע��
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

// Cdbc_demoDlg �Ի���
class Cdbc_demoDlg : public CDialog
{
// ����
public:
	Cdbc_demoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DBC_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	// �Ƿ������ͺ�CAN��
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
	CCycleBuffer<VCI_CAN_OBJ> m_frmBuffer;//��Ž��յ���֡
	std::vector<DBCMessage> m_vMsg;//���dbc�ļ���������Ϣ
	std::map<int, DBCMessage> m_vMultiMsg;//����շ��Ķ�֡��Ϣ
	double m_nSglValue;
	int m_nCurMsgIndex;
	int m_nLastSglIndex;
	int m_nCurObjIndex;
	Ctx m_ctx;//�ص�������, ��Ϊ�ص������Ĳ���

    CEdit m_Edit;  
    int m_Item;  //ҪEdit��  
    int m_SubItem; //ҪEdit��  
    int rowCount;  //������  
    int m_ComItem; // Ҫ����Combo Box ����   
    int m_ComSubItem; //Ҫ����Combo Box���� Ϊ���������������,���������bool����Ҳһ��  
    bool  needSave; //���ڱ����־,����й���д��ΪTURE,���´α༭ʱ����ԭ����λ������ԭ�����������  
    bool  ComneedSave;  
    //afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);  
    CComboBox m_comBox;  
	void SloveMsg( const DBCMessage& msg, int mID);
	void CreatFile_1();
	ONEBYTE oneWriteByte;

	CString DBCname, DBCBaseFile;
	int MsgNum;
public:
	// �����������
	CComboBox m_NMType;
};
