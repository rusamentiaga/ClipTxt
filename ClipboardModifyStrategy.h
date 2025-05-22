#pragma once
#include <string>
#include <algorithm>

// Clase base para implementar el patrón de diseño estrategia
class ClipboardModifyStrategy
{
protected:
	std::wstring Trim(const std::wstring& str) {
		const auto strBegin = std::find_if_not(str.begin(), str.end(), iswspace);
		const auto strEnd = std::find_if_not(str.rbegin(), str.rend(), iswspace).base();

		if (strBegin >= strEnd)
			return L""; // All spaces or empty

		return std::wstring(strBegin, strEnd);
	}

	virtual std::wstring ReplaceChar(std::wstring str, wchar_t src, wchar_t dest)
	{
		std::wstring newstr(str);
		std::replace(newstr.begin(), newstr.end(), src, dest);

		return Trim(newstr);
	}

public:
	virtual std::wstring ModifyString(std::wstring str) = 0;
};

class DontModify: public ClipboardModifyStrategy
{
public:
	virtual std::wstring ModifyString(std::wstring str)
	{
		return str;
	}
};