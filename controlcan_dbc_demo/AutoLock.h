#ifndef _AUTOLOCK_H_
#define _AUTOLOCK_H_

class CLock
{
public:
	CLock()
	{
		InitializeCriticalSection(&cs);
	}
	~CLock()
	{
		DeleteCriticalSection(&cs);
	}
	void Lock()
	{
		EnterCriticalSection(&cs);
	}
	void UnLock()
	{
		LeaveCriticalSection(&cs);
	}
private:
	CRITICAL_SECTION cs;
};

class CAutoLock
{
public:
	CAutoLock(CLock* pLock)
		: m_pLock(pLock)
	{
		m_pLock->Lock();
	}
	~CAutoLock()
	{
		m_pLock->UnLock();
	}
private:
	CLock* m_pLock;
};

#endif //_AUTOLOCK_H_