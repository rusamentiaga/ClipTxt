#pragma once
#include "clipboardmodifystrategy.h"

class ParagraphCase: public ClipboardModifyStrategy
{
protected:
	void UpperAfterChar(std::wstring& newstr, wchar_t charac)
	{
		std::string::size_type lastPos = 0;
		while ((lastPos = newstr.find(charac, lastPos + 1)) != std::string::npos)
		{
			for (std::string::size_type i = lastPos + 1; i < newstr.length(); i++)
			{
				if (IsCharAlpha(newstr[i]))
				{
					newstr[i] = toupper(newstr[i]);
					break;
				}
			}
		}
	}

public:
	virtual std::wstring ModifyString(std::wstring str)
	{
		if (str.empty())
			return str;

		int (*pf)(int) = tolower; 
		std::wstring newstr(str);

		std::transform(newstr.begin(), newstr.end(), newstr.begin(), pf); 
		newstr[0] = toupper(newstr[0]);
		UpperAfterChar(newstr, L'.');
		UpperAfterChar(newstr, L'\n');

		return newstr;
	}
};
