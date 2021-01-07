//。cpp文件

#include "stdafx.h"

#include "StdioFileEx.h"

//在UCS 编码中有一个叫做"ZERO WIDTH NO-BREAK SPACE"的字符，它的编码是FEFF。而FFFE在UCS中是不存在的字符，

//所以不应该出现在实际传输中。UCS规范建议我们在传输字节流前，先传输字符"ZERO WIDTH NO-BREAK SPACE"。这样

//如果接收者收到FEFF，就表明这个字节流是Big-Endian的；如果收到FFFE，就表明这个字节流是Little-Endian的。

//因此字符"ZERO WIDTH NO-BREAK SPACE"又被称作BOM。

//UTF-8不需要BOM来表明字节顺序，但可以用BOM来表明编码方式。字符"ZERO WIDTH NO-BREAK SPACE"的UTF-8编码是

//EF BB BF。所以如果接收者收到以EF BB BF开头的字节流，就知道这是UTF-8编码了。

//Windows就是使用BOM来标记文本文件的编码方式的。

//有些老的浏览器和文本编辑器不支持BOM。

#define UNICODE_BOM        0xFEFF//Unicode "byte order mark" which goes at start of file

CStdioFileEx::CStdioFileEx(): CStdioFile()

{

m_bIsUnicodeText = false;

}

CStdioFileEx::CStdioFileEx(LPCTSTR lpszFileName,UINT nOpenFlags)

:CStdioFile(lpszFileName, PreprocessFlags(lpszFileName, nOpenFlags))

{

}

BOOL CStdioFileEx::Open(LPCTSTR lpszFileName,UINT nOpenFlags,CFileException* pError /*=NULL*/)

{

PreprocessFlags(lpszFileName, nOpenFlags);

return CStdioFile::Open(lpszFileName, nOpenFlags, pError);

}

BOOL CStdioFileEx::ReadString(CString& rString)

{

#ifdef _UNICODE

return ReadWideString(rString);

#else

return ReadAnsiString(rString);

#endif

}

BOOL CStdioFileEx::ReadWideString(CStringW& rString)

{

_ASSERTE(m_pStream);

rString = L"";      // empty string without deallocating

if(m_bIsUnicodeText)

{

    // If at position 0, discard byte-order mark before reading

    if(GetPosition() == 0)

    {

     wchar_t bom;

     Read(&bom, sizeof(wchar_t));

    }

    const int nMaxSize = 128;

    LPWSTR lpsz = rString.GetBuffer(nMaxSize);

    LPWSTR lpszResult;

    int nLen = 0;

    for (;;)

    {

     lpszResult = fgetws(lpsz, nMaxSize+1, m_pStream);

     rString.ReleaseBuffer();

     // handle error/eof case

     if (lpszResult == NULL && !feof(m_pStream))

     {

      Afx_clearerr_s(m_pStream);

      AfxThrowFileException(CFileException::genericException, _doserrno,

       m_strFileName);

     }

     // if string is read completely or EOF

     if (lpszResult == NULL ||

      (nLen = (int)lstrlenW(lpsz)) < nMaxSize ||

      lpsz[nLen-1] == '\n')

      break;

     nLen = rString.GetLength();

     lpsz = rString.GetBuffer(nMaxSize + nLen) + nLen;

    }

    //remove crlf if exist.

    nLen = rString.GetLength();

    if (nLen > 1 && rString.Mid(nLen-2) == L"\r\n")

    {

     rString.GetBufferSetLength(nLen-2);

    }

    return rString.GetLength() > 0;

}

else

{

    CStringA ansiString;

    BOOL bRetval = ReadAnsiString(ansiString);

    //setlocale(LC_ALL, "chs_chn.936");//no need

    rString = ansiString;

    return bRetval;

}

}

BOOL CStdioFileEx::ReadAnsiString(CStringA& rString)

{

_ASSERTE(m_pStream);

rString = "";      // empty string without deallocating

if(!m_bIsUnicodeText)

{

    const int nMaxSize = 128;

    LPSTR lpsz = rString.GetBuffer(nMaxSize);

    LPSTR lpszResult;

    int nLen = 0;

    for (;;)

    {

     lpszResult = fgets(lpsz, nMaxSize+1, m_pStream);

     rString.ReleaseBuffer();

     // handle error/eof case

     if (lpszResult == NULL && !feof(m_pStream))

     {

      Afx_clearerr_s(m_pStream);

      AfxThrowFileException(CFileException::genericException, _doserrno,

       m_strFileName);

     }

     // if string is read completely or EOF

     if (lpszResult == NULL ||

      (nLen = (int)lstrlenA(lpsz)) < nMaxSize ||

      lpsz[nLen-1] == '\n')

      break;

     nLen = rString.GetLength();

     lpsz = rString.GetBuffer(nMaxSize + nLen) + nLen;

    }

    //remove crlf if exist.

    nLen = rString.GetLength();

    if (nLen > 1 && rString.Mid(nLen-2) == "\r\n")

    {

     rString.GetBufferSetLength(nLen-2);

    }

    return rString.GetLength() > 0;

}

else

{

    CStringW wideString;

    BOOL bRetval = ReadWideString(wideString);

    //setlocale(LC_ALL, "chs_chn.936");//no need

    rString = wideString;

    return bRetval;

}

}

// Purpose:    Writes string to file either in Unicode or multibyte, depending on whether the caller specified the

//       CStdioFileEx::modeWriteUnicode flag. Override of base class function.

void CStdioFileEx::WriteString(LPCTSTR lpsz)

{

#ifdef _UNICODE

WriteWideString(lpsz);

#else

WriteAnsiString(lpsz);

#endif

}

void CStdioFileEx::WriteWideString(LPCWSTR lpsz)

{

ASSERT(lpsz != NULL);

if (lpsz == NULL)

{

    AfxThrowInvalidArgException();

}

if(m_bIsUnicodeText)

{

    ASSERT(m_pStream != NULL);

    // If writing Unicode and at the start of the file, need to write byte mark

    if(GetPosition() == 0)

    {

     wchar_t cBOM = (wchar_t)UNICODE_BOM;

     CFile::Write(&cBOM, sizeof(wchar_t));

    }

    if (fputws(lpsz, m_pStream) == _TEOF)

     AfxThrowFileException(CFileException::diskFull, _doserrno, m_strFileName);

}

else

{

    USES_CONVERSION;

    WriteAnsiString(CW2A(lpsz));

}

}

void CStdioFileEx::WriteAnsiString(LPCSTR lpsz)

{

ASSERT(lpsz != NULL);

if (lpsz == NULL)

{

    AfxThrowInvalidArgException();

}

if(!m_bIsUnicodeText)

{

    ASSERT(m_pStream != NULL);

    if (fputs(lpsz, m_pStream) == _TEOF)

     AfxThrowFileException(CFileException::diskFull, _doserrno, m_strFileName);

}

else

{

    USES_CONVERSION;

    WriteWideString(CA2W(lpsz));

}

}

UINT CStdioFileEx::PreprocessFlags(const CString& sFilePath, UINT& nOpenFlags)

{

m_bIsUnicodeText = false;

// If we have writeUnicode we must have write or writeRead as well

if (nOpenFlags & CStdioFileEx::modeWriteUnicode)

{

    ASSERT(nOpenFlags & CFile::modeWrite || nOpenFlags & CFile::modeReadWrite);

    m_bIsUnicodeText = true;

}

// If reading in text mode and not creating...

else if (nOpenFlags & CFile::typeText && !(nOpenFlags & CFile::modeCreate) && !(nOpenFlags & CFile::modeWrite ))

{

    m_bIsUnicodeText = IsFileUnicode(sFilePath);

}

//如果要读写Unicode格式的文本文件, 必须切换到typeBinary方式, 因为这会影响fputws/fgetws的工作方式(具体情况参考MSDN)。

if (m_bIsUnicodeText)

{

    nOpenFlags &= ~(CFile::typeText);

    nOpenFlags |= CFile::typeBinary;

}

return nOpenFlags;

}

// Purpose:    Determines whether a file is Unicode by reading the first character and detecting

//       whether it's the Unicode byte marker.

bool CStdioFileEx::IsFileUnicode(const CString& sFilePath)

{

CFile      file;

wchar_t     cFirstChar;

CFileException exFile;

bool      bIsUnicode = false;

// Open file in binary mode and read first character

if (file.Open(sFilePath, CFile::typeBinary | CFile::modeRead, &exFile))

{

    // If byte is Unicode byte-order marker, let's say it's Unicode

    if (file.Read(&cFirstChar, sizeof(wchar_t)) > 0 && cFirstChar == (wchar_t)UNICODE_BOM)

    {

     bIsUnicode = true;

    }

    file.Close();

}

else

{

    // Handle error here if you like

}

return bIsUnicode;

}

unsigned long CStdioFileEx::GetCharCount()

{

int      nCharSize;

unsigned long nByteCount, nCharCount = 0;

if (m_pStream)

{

    // Get size of chars in file

    nCharSize = m_bIsUnicodeText ? sizeof(wchar_t): sizeof(char);

    // If Unicode, remove byte order mark from count

    nByteCount = (unsigned long)GetLength();

    if (m_bIsUnicodeText)

    {

     nByteCount = nByteCount - sizeof(wchar_t);

    }

    // Calc chars

    nCharCount = (nByteCount / nCharSize);

}

return nCharCount;

}