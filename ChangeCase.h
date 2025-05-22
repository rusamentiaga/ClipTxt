#pragma once
#include "clipboardmodifystrategy.h"

class ChangeCase: public ClipboardModifyStrategy
{
protected:
	std::wstring DoChangeCase(std::wstring str, int (*pf)(int))
	{
		std::wstring newstr(str);
		std::transform(newstr.begin(), newstr.end(), newstr.begin(), pf); 

		return newstr;
	}
public:
	virtual std::wstring ModifyString(std::wstring str) = 0;
};

class LowerCase: public ChangeCase
{
public:
	virtual std::wstring ModifyString(std::wstring str)
	{
		return DoChangeCase(str, tolower);
	}
};

class UpperCase: public ChangeCase
{
public:
	virtual std::wstring ModifyString(std::wstring str)
	{
		return DoChangeCase(str, toupper);
	}
};

class ChangeSpaceTab : public ClipboardModifyStrategy
{
public:
	virtual std::wstring ModifyString(std::wstring str)
	{
		return ReplaceChar(str, L' ', L'\t');
	}
};

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