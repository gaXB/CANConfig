#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <string>
using std::string;
using std::wstring;

namespace zy
{
	//宽字符转为多字节字符
	string W2AEx( LPCTSTR wzsStr );
}

#endif //_UTILITY_H_