#pragma once
#include "ClipboardModifyStrategy.h"

class ChangeSpaceTab : public ClipboardModifyStrategy
{
public:
	virtual std::wstring ModifyString(std::wstring str)
	{
		return ReplaceChar(str, L' ', L'\t');
	}
};

class ChangeTabSpace : public ClipboardModifyStrategy
{
public:
	static const int SpacesPerTab = 3;

	virtual std::wstring ModifyString(std::wstring str)
	{
		std::wstring newstr;
		for (wchar_t c : str)
		{
			if (c == L'	')
				newstr.append(SpacesPerTab, L' ');
			else
				newstr += c;
		}

		return Trim(newstr);
	}
};
