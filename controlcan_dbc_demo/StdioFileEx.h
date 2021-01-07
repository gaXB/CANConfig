#pragma once

class CStdioFileEx: public CStdioFile

{

public:

CStdioFileEx();

CStdioFileEx( LPCTSTR lpszFileName, UINT nOpenFlags );

virtual BOOL Open( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL );

virtual BOOL ReadString(CString& rString);

BOOL ReadWideString(CStringW& rString);

BOOL ReadAnsiString(CStringA& rString);

virtual void WriteString(LPCTSTR lpsz);

void WriteWideString(LPCWSTR lpsz);

void WriteAnsiString(LPCSTR lpsz);

bool IsUnicodeFormat() {return m_bIsUnicodeText;}

unsigned long GetCharCount();

// Additional flag to allow Unicode text format writing

enum {modeWriteUnicode = 0x100000};

static bool IsFileUnicode(const CString& sFilePath);

protected:

UINT PreprocessFlags(const CString& sFilePath, UINT& nOpenFlags);

bool    m_bIsUnicodeText;

};