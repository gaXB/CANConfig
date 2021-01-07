#include "stdafx.h"
#include "utility.h"
namespace zy
{
	string W2AEx( LPCTSTR wzsStr )
	{
		string strRet;
		wstring strTemp = wstring(wzsStr);
		int nLength=WideCharToMultiByte(CP_ACP,0,strTemp.c_str(),strTemp.length(),NULL,0,NULL,NULL);
		char* pAnsi = NULL;
		if (nLength > 0)
		{
			pAnsi = new char[nLength+1];
		}
		if (pAnsi)
		{
			WideCharToMultiByte(CP_ACP,0,strTemp.c_str(),strTemp.length(),pAnsi,nLength,NULL,NULL);
			pAnsi[nLength]=0;
			strRet = string(pAnsi);
			delete[] pAnsi;
		}
		return strRet;
	}
};
