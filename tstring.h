#pragma once

#include <windows.h>
#include <sstream>
#include <string>
#include <assert.h>
#include <tchar.h>

namespace AC
{
	#ifdef _UNICODE
		typedef std::wstring tstring;
		typedef std::wostringstream tostringstream;
		#define AnsiTotstring(s) AC::StrUtil::AnsiToWideString(s, lstrlenA(s))
		#define WideTotstring(s) AC::tstring(s)
	#else
		typedef std::string tstring;
		typedef std::ostringstream tostringstream;
		#define AnsiTotstring(s) AC::tstring(s)
		#define WideTotstring(s) AC::StrUtil::WideToAnsiString(s, lstrlenW(s))
	#endif

	class StrUtil
	{
	public:
		static std::wstring AnsiToWideString(const char* pStr, int len)
		{
			assert(pStr != NULL); 
			assert(len >= 0); 

			int nChars = MultiByteToWideChar(CP_ACP , 0 , pStr , len , NULL , 0); 
			if (len == -1)
				nChars--; 
			if (nChars == 0)
				return L"";

			std::wstring buf ;
			buf.resize(nChars) ; 
			MultiByteToWideChar(CP_ACP, 0, pStr, len, 
				const_cast<wchar_t*>(buf.c_str()), nChars ) ; 

			return buf ;
		}

		static std::string WideToAnsiString(const wchar_t* pStr , size_t len)
		{
			assert(pStr != NULL); 
			assert(len >= 0); 

			int nChars = WideCharToMultiByte(CP_ACP, 0, pStr, (int)len, NULL, 0, NULL, NULL) ; 
			if (len == -1)
				nChars--; 
			if (nChars == 0)
				return "";

			std::string buf ;
			buf.resize(nChars);
			WideCharToMultiByte(CP_ACP, 0, pStr, (int)len, const_cast<char*>(buf.c_str()), nChars, NULL, NULL); 

			return buf ; 
		}
	};
}
