#pragma once
#include "clipboardmodifystrategy.h"
#include <sstream>
#include <algorithm>

class PaperTittleCamelCase :
	public ClipboardModifyStrategy
{
public:
	virtual std::wstring ModifyString(std::wstring str)
	{
		LPCTSTR AlwaysLowerWords[] = {
			L"a", L"ante", L"bajo", L"cabe", L"con", L"contra", L"de", L"desde", L"durante", L"en", 
			L"entre", L"hacia", L"hasta", L"mediante", L"para", L"por", L"según", L"sin", L"so", 
			L"sobre", L"tras", L"vía", L"las", L"los", L"about", L"Above", L"About", L"Across", L"After", 
			L"Along", L"Amid", L"Among", L"Around", L"As", L"At", L"Before", L"Behind", L"Below", L"Beneath", 
			L"Between", L"By", L"Because", L"Down", L"During", L"Except", L"For", L"From", L"In", L"Into", 
			L"Inside", L"Instead", L"Near", L"Next", L"Of", L"Off", L"On", L"Outside", L"Past", L"Out", 
			L"Round", L"Since", L"Than", L"Through", L"Till", L"To", L"Under", L"Underneath", L"Until", 
			L"Unto", L"Upon", L"Without", L"With", L"Within", L"The", L"e", L"la", L"el" , L"and", L"y", 
			L"del"
		};
		const int NumAlwaysLowerWords = sizeof(AlwaysLowerWords)/sizeof(LPCTSTR);

		unsigned i;
		std::wstring newstr;
		int (*pf)(int) = tolower; 
		std::wistringstream iss(str);

		do
		{
			std::wstring sub;
			iss >> sub;
			if (!sub.empty())
			{
				std::transform(sub.begin(), sub.end(), sub.begin(), pf); 

				for (i = 0; i < NumAlwaysLowerWords; i++)
				{
					std::wstring low = AlwaysLowerWords[i];
					std::transform(low.begin(), low.end(), low.begin(), pf); 

					if (sub == low)
						break;
				}
				if (i == NumAlwaysLowerWords)
					sub[0] = toupper(sub[0]);

				if (!newstr.empty())
					newstr += _T(" ") + sub;
				else
				{
					// La primera palabra si se pone en mayus
					if (i != NumAlwaysLowerWords)
						sub[0] = toupper(sub[0]);

					newstr = sub;
				}
			}
		} while (iss);

		// La primera letra siempre en mayus
		if (!newstr.empty())
			newstr[0] = toupper(newstr[0]);

		return newstr;
	}
};
