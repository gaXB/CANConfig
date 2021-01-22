#ifndef _IDBCMANAGER_H_
#define _IDBCMANAGER_H_

#ifdef WIN32
    #define FUNCCALL __stdcall
#else
    #define FUNCCALL
#endif

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

typedef unsigned int     uint32;
typedef unsigned int     DBCHandle;
typedef unsigned char    uint8;
typedef unsigned short   uint16;
typedef unsigned long long uint64;
typedef uint8 ERR_CODE;
//error code
#define ERR_SUCCESS            0
#define ERR_FAILED             1
#define ERR_MULTI_TRANSMITTING 2 //��֡���䷢����

#define INVALID_DBC_HANDLE        -1 // ��Ч��DBC���

#define _MAX_FILE_PATH_           260 //��ļ�·��
#define _DBC_NAME_LENGTH_         127  //���������
#define _DBC_COMMENT_MAX_LENGTH_  200 //ע�������
#define _DBC_UNIT_MAX_LENGTH_     10  //��λ�����
#define _DBC_SIGNAL_MAX_COUNT_    128 //һ����Ϣ���е��źŵ������Ŀ

//multiplexer type
#define MUTIPLEXER_NONE     0
#define MUTIPLEXER_M_VALUE  1
#define MUTIPLEXER_M_SWITCH 2

struct DBCSignal
{
    uint32 nStartBit; // ��ʼλ
    uint32 nLen;	// λ����
    double nFactor; // ת������
    double nOffset;	// ת��ƫ�� ʵ��ֵ=ԭʼֵ*nFactor+nOffset
    double nMin;    // ��Сֵ
    double nMax;	// ���ֵ
    double nValue;  //ʵ��ֵ
    uint64 nRawValue;//ԭʼֵ
    bool is_signed; //1:�з�������, 0:�޷���
    bool is_motorola;//�Ƿ�Ħ��������ʽ
    uint8 multiplexer_type;//see 'multiplexer type' above
    uint8 val_type;//0:integer, 1:float, 2:double
    uint32 multiplexer_value;
    char unit[_DBC_UNIT_MAX_LENGTH_+1];//��λ
    char strName[_DBC_NAME_LENGTH_+1];  //����
    char strComment[_DBC_COMMENT_MAX_LENGTH_+1];  //ע��
    char strValDesc[_DBC_NAME_LENGTH_+1];  //ֵ����
};

struct DBCMessage
{
    uint32 nSignalCount; //�ź�����
    uint32 nID;
    uint32 nSize;	//��Ϣռ���ֽ���Ŀ
    double nCycleTime;//��������
    uint8 nExtend; //1:��չ֡, 0:��׼֡
    DBCSignal vSignals[_DBC_SIGNAL_MAX_COUNT_]; //�źż���
    char strName[_DBC_NAME_LENGTH_+1];  //����
    char strComment[_DBC_COMMENT_MAX_LENGTH_+1];    //ע��
};

#define PROTOCOL_J1939   0
#define PROTOCOL_OTHER 1
struct FileInfo
{
    char strFilePath[_MAX_FILE_PATH_+1]; //dbc�ļ�·��
    uint8 type; //dbc��Э������, j1939ѡ��PROTOCOL_J1939, ����Э��ѡ��PROTOCOL_OTHER
    uint8 merge;//1:��������е�����, ��֧�ּ��ض���ļ�;0�����ԭ��������
};

//ֵ�뺬���, ����3 "Not Supported" 2 "Error" 1 "Not Defined"
struct ValDescPair
{
    double value;
    char desc[_DBC_NAME_LENGTH_+1];
};

/*
����˵������ûʵ�ʷ������ݹ���, ֻ�ṩ���͵�����,
          ��������Ҫʵ�ָûص���������ʵ�����ݷ��͡�
��    ��[in]��ctx-�ص�������������
��    ��[in]��pObj-Ҫ���͵�����
�� �� ֵ��true���ͳɹ�
*/
typedef bool (FUNCCALL *OnSend)(void* ctx, void* pObj);

/*
����˵������֡������ɺ�ص�, ������漰��֡����Ҳ���Բ�ʵ�ָûص�
��    ��[in]��ctx-�ص�������������
��    ��[in]��pMsg-���ص�DBC��Ϣ
��    ��[in]��data-��֡��ʵ������
��    ��[in]��nLen-��֡���ݵ��ֽ���Ŀ
��    ��[in]��nDirection-��֡�Ĵ��䷽�� 1:����, 0:����
�� �� ֵ��void
*/
typedef void (FUNCCALL *OnMultiTransDone)(void* ctx, DBCMessage* pMsg, uint8* data, uint16 nLen, uint8 nDirection);

/*
����˵������ʼ������ģ�顣
�� �� ֵ��INVALID_DBC_HANDLE:��ʼ��ʧ��
          ����ֵ:��ʼ���ɹ�
          �����ֵ������ʹ�õ���
*/
EXTERN_C  DBCHandle FUNCCALL DBC_Init();

/*
����˵�����ͷ���Դ, ��DBC_Init���ʹ��
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
*/
EXTERN_C  void FUNCCALL DBC_Release( DBCHandle hDBC );

/*
����˵��������DBC��ʽ�ļ���
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[in]��pFileInfo-�ļ���Ϣ, �ο�struct FileInfo
�� �� ֵ��true���سɹ�
*/
EXTERN_C  bool FUNCCALL DBC_LoadFile( DBCHandle hDBC, const FileInfo* pFileInfo );

/*
����˵������ȡ��һ����Ϣ��
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[out]��pMsg �洢��Ϣ����Ϣ
�� �� ֵ��true��ʾ�ɹ�
*/
EXTERN_C  bool FUNCCALL DBC_GetFirstMessage( DBCHandle hDBC, DBCMessage* pMsg );

/*
����˵������ȡ��һ����Ϣ��
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[out]��pMsg �洢��Ϣ����Ϣ
�� �� ֵ��true��ʾ�ɹ�
*/
EXTERN_C  bool FUNCCALL DBC_GetNextMessage( DBCHandle hDBC, DBCMessage* pMsg );

/*
����˵��������ID��ȡ��Ϣ��
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[in]��nID-��Ϣ��ID
��    ��[out]��pMsg �洢��Ϣ����Ϣ
�� �� ֵ��true��ʾ�ɹ�
*/
EXTERN_C  bool FUNCCALL DBC_GetMessageById( DBCHandle hDBC, uint32 nID, DBCMessage* pMsg );
/*
����˵������ȡ��Ϣ������
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
�� �� ֵ����Ϣ����
*/
EXTERN_C  uint32 FUNCCALL DBC_GetMessageCount( DBCHandle hDBC );

/*
����˵��������DBCЭ�����֡����
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[in]��pObj ԭʼ֡����
��    ��[out]��pMsg �������
�� �� ֵ��true��ʾ�ɹ�
*/
EXTERN_C  bool FUNCCALL DBC_Analyse( DBCHandle hDBC, const void* pObj, DBCMessage* pMsg );

/*
����˵������������Ҫ���øú����ѽ��յ���֡���ݴ�����,
          �漰��֡�������Ҫ��������, �����޷�ʵ���豸������
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[in]��pObj ԭʼ֡����
�� �� ֵ��void
*/
EXTERN_C  void FUNCCALL DBC_OnReceive( DBCHandle hDBC, const void* pObj );

/*
����˵��������ʵ�ʷ������ݵĴ�����, �漰���ݷ���ʱ�������ã�ֻ��Ҫ����һ�Ρ�
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[in]��sender�ص�����ָ��
��    ��[in]��ctx-�ص�������������
�� �� ֵ��void
*/
EXTERN_C  void FUNCCALL DBC_SetSender( DBCHandle hDBC, OnSend sender, void* ctx );

/*
����˵�������ô����֡�������ݵĻص�������ֻ��Ҫ����һ��
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[in]��OnMultiTransDone�ص�����ָ��
��    ��[in]��ctx-�ص�������������
�� �� ֵ��void
*/
EXTERN_C  void FUNCCALL DBC_SetOnMultiTransDoneFunc( DBCHandle hDBC, OnMultiTransDone func, void* ctx);
/*
����˵��������DBC��Ϣ��
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[in]��pMsg Ҫ���͵���Ϣ
�� �� ֵ���ο�//error code
*/
EXTERN_C  ERR_CODE FUNCCALL DBC_Send( DBCHandle hDBC, const DBCMessage* pMsg );

/*
����˵������ȡ�����źŵ�ֵ�뺬��Ը�����
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[in]��msg_id, ��ϢID
��    ��[in]��signal_name, �ź���
�� �� ֵ��ֵ�뺬��Ը���
*/
EXTERN_C  uint32 FUNCCALL DBC_GetValDescPairCount( DBCHandle hDBC, uint32 msg_id, char* signal_name);
/*
����˵������ȡֵ�뺬��ԡ�
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[in]��msg_id, ��ϢID
��    ��[in]��signal_name, �ź���
��    ��[in,out]��pair ����,�����߹���,��СΪDBC_GetValDescPairCount����ֵ
�� �� ֵ��true��ʾ�ɹ�
*/
EXTERN_C  void FUNCCALL DBC_GetValDescPair( DBCHandle hDBC, uint32 msg_id, char* signal_name, ValDescPair* pair);
/*
����˵��������ԭʼ���ݽ���ΪDBCMessage
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[out]��pMsg �������
��    ��[in]��pObj ԭʼ֡����
��    ��[in]��nCount ԭʼ֡���ݸ���
�� �� ֵ��true��ʾ�ɹ�
*/
EXTERN_C  bool FUNCCALL DBC_Decode( DBCHandle hDBC, DBCMessage* pMsg, const void* pObj, uint32 nCount );
/*
����˵��������DBCMessage����Ϊԭʼ����
��    ��[in]��hDBC-���, DBC_Load�ķ���ֵ
��    ��[out]��pObj �����ԭʼ���ݻ�����
��    ��[in,out]��pObj ��������С, ����ʱΪʵ��ԭʼ���ݸ���
��    ��[in]��pMsg Ҫ�������Ϣ
�� �� ֵ��true��ʾ�ɹ�
*/
EXTERN_C  bool FUNCCALL DBC_Encode( DBCHandle hDBC, void* pObj, uint32* nCount, const DBCMessage* pMsg );

#endif // _IDBCMANAGER_H_
