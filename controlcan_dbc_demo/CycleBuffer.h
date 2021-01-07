// ICANObjBuffer.h: interface for the CICANObjBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CYCLEBUFFER_H__6B69BDE5_C980_4835_98CF_EAD81E1D3A12__INCLUDED_)
#define AFX_CYCLEBUFFER_H__6B69BDE5_C980_4835_98CF_EAD81E1D3A12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AutoLock.h"

#define MAX_CYCLEOBJBUF		1024
#undef SAFE_DELETE_AR
#define SAFE_DELETE_AR(ptr) \
	if (ptr) { delete [] ptr; ptr = NULL; }

//##ModelId=43F51EAC02E2
template<class T>
class CCycleBuffer
{
public:
	int m_iChanged;

	//##ModelId=43F51EAC0301
	CLock m_sec;
	//##ModelId=43F51EAC0314
	T* m_pobj;
	//##ModelId=43F51EAC0332
	int m_len;
	//##ModelId=43F51EAC0350
	int m_off;
	//##ModelId=43F51EAC0364
	int m_validlen;
	//##ModelId=43F51EAC0378
	CCycleBuffer():m_pobj(NULL),m_len(0),m_off(0),m_validlen(0),m_iChanged(0)
	{
	}
	CCycleBuffer(int len):m_pobj(NULL),m_len(len),m_off(0),m_validlen(0) 
	{
	}
	//##ModelId=43F51EAC0382
	virtual ~CCycleBuffer()
	{
		SAFE_DELETE_AR(m_pobj);
	}
	void Release()
	{
		CAutoLock autolock(&m_sec);
		SAFE_DELETE_AR(m_pobj);
	}
	//##ModelId=43F51EAC0384
	void ReAllocate(int maxlen)
	{
		CAutoLock autolock(&m_sec);
		if(maxlen<=m_len&&m_pobj!=NULL)
			return;
		T* pnewobj=new T[maxlen];
		if(m_pobj!=NULL)
		{
			int tmp1=((m_validlen+m_off<=m_len)?m_validlen:(m_len-m_off));
			memcpy(pnewobj,m_pobj+m_off,tmp1*sizeof(T));
			if(m_validlen+m_off>m_len)
				memcpy(pnewobj+tmp1,m_pobj,
					(m_validlen+m_off-m_len)*sizeof(T));
			SAFE_DELETE_AR(m_pobj);
		}
		m_pobj=pnewobj;
		m_len=maxlen;
		m_off=0;
		m_iChanged=1;
	}
	//##ModelId=43F51EAC038D
	void AddObj(T* pobj,int objlen)
	{
		CAutoLock autolock(&m_sec);
		if(m_pobj==NULL)
		{
			ReAllocate(m_len==0?MAX_CYCLEOBJBUF:m_len);
		}
		if(objlen>m_len)
		{
			objlen=m_len;
// 			pobj+=objlen-m_len;
// 			objlen=m_len;
		}
		int start=m_validlen+m_off;
		int itmp=m_off+m_len-start-objlen;
		if(itmp<0)
		{
			m_off-=itmp;
			if(m_off>=m_len)
				m_off-=m_len;
		}
		if(start>=m_len)
			start-=m_len;
		m_validlen+=objlen;
		if(m_validlen>m_len)
			m_validlen=m_len;

		int tmp1=min(m_len-start,objlen);
		memcpy(m_pobj+start,pobj,tmp1*sizeof(T));
		if(tmp1<objlen)
		{
			memcpy(m_pobj,pobj+tmp1,(objlen-tmp1)*sizeof(T));
		}
		m_iChanged=1;
	}
	
	//##ModelId=43F51EAC03A1
	void CopyObj(T* pobj,int& objlen)
	{
		if(m_validlen==0)
		{
			objlen=0;
			return;
		}

		CAutoLock autolock(&m_sec);
		int getlen=min(objlen,m_validlen);
		int tmp1=(getlen+m_off>m_len)?(m_len-m_off):(getlen);
		int off=m_off;
		memcpy(pobj,m_pobj+m_off,tmp1*sizeof(T));

		m_off+=tmp1;
		if(m_off==m_len)
			m_off=0;
		if(tmp1<getlen)
		{
			memcpy(pobj+tmp1,m_pobj+m_off,(getlen-tmp1)*
				sizeof(T));
			m_off+=getlen-tmp1;
		}
		objlen=getlen;
		m_off=off;
	}

	//##ModelId=43F51EAC03B4
	void GetObj(T* pobj,int& objlen)
	{
		if(m_validlen==0)
		{
			objlen=0;
			return;
		}

		CAutoLock autolock(&m_sec);
		int getlen=min(objlen,m_validlen);
		int tmp1=(getlen+m_off>m_len)?(m_len-m_off):(getlen);
		memcpy(pobj,m_pobj+m_off,tmp1*sizeof(T));

		m_off+=tmp1;
		if(m_off==m_len)
			m_off=0;
		if(tmp1<getlen)
		{
			memcpy(pobj+tmp1,m_pobj+m_off,(getlen-tmp1)*
				sizeof(T));
			m_off+=getlen-tmp1;
		}
		m_validlen-=getlen;
		objlen=getlen;
		m_iChanged=1;
	}

	//##ModelId=43F51EAC03C8
	void Clear()
	{
		CAutoLock autolock(&m_sec);
		m_validlen=0;
		m_iChanged=1;
	}
	//##ModelId=43F51EAC03D2
	int GetLen()
	{
		return m_validlen;
	}
	int GetSize()
	{
		return m_len;
	}
	void SetLen(int len)
	{
		CAutoLock autolock(&m_sec);
		m_validlen=0;
		m_len=len;
		SAFE_DELETE_AR(m_pobj);
		m_iChanged=1;
	}

	int CopySingleObjbyPos(T& obj,int pos)
	{
		CAutoLock autolock(&m_sec);
		if(pos>=m_validlen)
			return 0;
		int tmp1=(pos+m_off>=m_len)?(m_off+pos-m_len):(pos+m_off);
		obj=*(m_pobj+tmp1);
		return 1;
	}

	void TryAllocateWhenFull()
	{
		CAutoLock autolock(&m_sec);
		if(m_validlen==m_len)
		{
			ReAllocate(m_len+MAX_CYCLEOBJBUF);
		}
	}

	int ReadChanged()
	{
		CAutoLock autolock(&m_sec);
		int changed=m_iChanged;
		m_iChanged=0;
		return changed;
	}
};

#endif // !defined(AFX_CYCLEBUFFER_H__6B69BDE5_C980_4835_98CF_EAD81E1D3A12__INCLUDED_)
