
// dbc_demo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cdbc_demoApp:
// �йش����ʵ�֣������ dbc_demo.cpp
//

class Cdbc_demoApp : public CWinAppEx
{
public:
	Cdbc_demoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cdbc_demoApp theApp;