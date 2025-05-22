#pragma once
#include "tstring.h"
#include <iomanip>

namespace AC
{
	class Util
	{
	public:
		static tstring StrError(DWORD ErrorCode)
		{
			LPVOID FailReason;

			if (FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL, ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				(LPTSTR) &FailReason, 0, NULL) == 0)
			{
				return _T("");
			}

			tstring FailReasonStr = (LPCTSTR)FailReason;
			LocalFree(FailReason);
			return FailReasonStr;
		}

		static tstring NowTime()
		{
			SYSTEMTIME LocalTime;
			tostringstream TimeStringStream;

			GetLocalTime(&LocalTime);
			TimeStringStream << _T("[");
			TimeStringStream <<	std::setfill(_T('0')) << std::setw(2) << LocalTime.wHour << _T(":"); 
			TimeStringStream <<	std::setfill(_T('0')) << std::setw(2) << LocalTime.wMinute << _T(":");
			TimeStringStream <<	std::setfill(_T('0')) << std::setw(2) << LocalTime.wSecond << _T("]");
			return TimeStringStream.str();
		}
	};
}
