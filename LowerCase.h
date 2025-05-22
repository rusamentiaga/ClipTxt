#pragma once
#include "clipboardmodifystrategy.h"

class LowerCase :
	public ClipboardModifyStrategy
{
public:
	virtual std::wstring ModifyString(std::wstring str)
	{
		std::wstring newstr(str);
		int (*pf)(int) = tolower; 

		std::transform(newstr.begin(), newstr.end(), newstr.begin(), pf); 
		return newstr;
	}
};
