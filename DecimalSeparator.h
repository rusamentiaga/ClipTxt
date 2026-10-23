#pragma once
#include "ClipboardModifyStrategy.h"

class CommaPerDotTrim: public ClipboardModifyStrategy
{

public:
	virtual std::wstring ModifyString(std::wstring str)
	{
		return ReplaceChar(str, L',', L'.');
	}
};

class DotPerComma: public ClipboardModifyStrategy
{

public:
	virtual std::wstring ModifyString(std::wstring str)
	{
		return ReplaceChar(str, L'.', L',');
	}
};

class RemoveDotsCommaPerDot: public ClipboardModifyStrategy
{

public:
	virtual std::wstring ModifyString(std::wstring str)
	{
       if (str.find(L',') == std::wstring::npos)
			return str;

		std::wstring newstr(str);

		newstr.erase(std::remove(newstr.begin(), newstr.end(), L'.'), newstr.end());
		std::replace(newstr.begin(), newstr.end(), L',', L'.');

		return Trim(newstr);
	}
};
