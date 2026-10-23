#pragma once
#include "PaperTittleCamelCase.h"
#include "ParagraphCase.h"
#include "ChangeCase.h"
#include "SpaceTab.h"
#include "DecimalSeparator.h"
#include <iostream>
#include <locale>
#include <string> 

// Reports a failed check with its location and keeps running the rest
#define CHECK(expr) Check((expr), #expr, __FILE__, __LINE__)

class UnitTest
{
	int m_Checks;
	int m_Failures;

	void Check(bool ok, const char* expr, const char* file, int line)
	{
		m_Checks++;
		if (!ok)
		{
			m_Failures++;
			std::cerr << file << "(" << line << "): FAILED: " << expr << std::endl;
		}
	}

public:

	UnitTest(void) : m_Checks(0), m_Failures(0)
	{
	}

	~UnitTest(void)
	{
	}


	void RunPaperTittleCamelCase()
	{
		PaperTittleCamelCase CamelCase;

		CHECK(CamelCase.ModifyString(L"Hola") == L"Hola");
		CHECK(CamelCase.ModifyString(L"Hola2") == L"Hola2");
		CHECK(CamelCase.ModifyString(L"HOLA") == L"Hola");		
		CHECK(CamelCase.ModifyString(L"HOLA HOLA") == L"Hola Hola");		
		CHECK(CamelCase.ModifyString(L"HOLA de HOLA") == L"Hola de Hola");	
		CHECK(CamelCase.ModifyString(L"HOLA PARA HOLA") == L"Hola para Hola");		
		CHECK(CamelCase.ModifyString(L"HOLA LAS HOLA") == L"Hola las Hola");		
		CHECK(CamelCase.ModifyString(L"HOLA THE HOLA") == L"Hola the Hola");		
		CHECK(CamelCase.ModifyString(L"THE HOLA THE HOLA") == L"The Hola the Hola");		
		CHECK(CamelCase.ModifyString(L"THE HOLA in HOLA") == L"The Hola in Hola");		
		CHECK(CamelCase.ModifyString(L"ALUMNOS DE INGENIERÍA TÉCNICA INFORMÁTICA") == L"Alumnos de Ingeniería Técnica Informática");
		CHECK(CamelCase.ModifyString(L"la INFORMÁTICA") == L"La Informática");
	}

	void RunUpperCase()
	{
		UpperCase UpCase;

		CHECK(UpCase.ModifyString(L"Hola") == L"HOLA");
		CHECK(UpCase.ModifyString(L"HOLA") == L"HOLA");
		CHECK(UpCase.ModifyString(L"hola") == L"HOLA");
	}

	void RunLowerCase()
	{
		LowerCase LowCase;

		CHECK(LowCase.ModifyString(L"Hola") == L"hola");
		CHECK(LowCase.ModifyString(L"HOLA") == L"hola");
		CHECK(LowCase.ModifyString(L"hola") == L"hola");
	}

	void RunParagraphCase()
	{
		ParagraphCase ParCase;

		CHECK(ParCase.ModifyString(L"Hola") == L"Hola");
		CHECK(ParCase.ModifyString(L"HOLA que tal") == L"Hola que tal");
		CHECK(ParCase.ModifyString(L"Hola. ADIOS") == L"Hola. Adios");
		CHECK(ParCase.ModifyString(L"Hola. ADIOS\n\nhasta luego") == L"Hola. Adios\n\nHasta luego");
		CHECK(ParCase.ModifyString(L"Hola. ADIOS\n\nítaca") == L"Hola. Adios\n\nÍtaca");
	}

	void RunEmptyString()
	{
		CHECK(PaperTittleCamelCase().ModifyString(L"") == L"");
		CHECK(ParagraphCase().ModifyString(L"") == L"");
		CHECK(UpperCase().ModifyString(L"") == L"");
		CHECK(LowerCase().ModifyString(L"") == L"");
	}

	void RunDontModify()
	{
		DontModify NoChange;

		// Default strategy: text is only re-copied as plain text, unchanged
		CHECK(NoChange.ModifyString(L"Hola") == L"Hola");
		CHECK(NoChange.ModifyString(L"  Hola\tque\r\ntal  ") == L"  Hola\tque\r\ntal  ");
		CHECK(NoChange.ModifyString(L"") == L"");
	}

	void RunChangeSpaceTab()
	{
		ChangeSpaceTab SpaceTab;

		CHECK(SpaceTab.ModifyString(L"a b c") == L"a\tb\tc");
		CHECK(SpaceTab.ModifyString(L"1  2") == L"1\t\t2");
		CHECK(SpaceTab.ModifyString(L"abc") == L"abc");
		CHECK(SpaceTab.ModifyString(L" a b ") == L"a\tb");
	}

	void RunChangeTabSpace()
	{
		ChangeTabSpace TabSpace;

		CHECK(TabSpace.ModifyString(L"a	b	c") == L"a   b   c");
		CHECK(TabSpace.ModifyString(L"1		2") == L"1      2");
		CHECK(TabSpace.ModifyString(L"abc") == L"abc");
		CHECK(TabSpace.ModifyString(L"a b") == L"a b");
		CHECK(TabSpace.ModifyString(L"	a	b	") == L"a   b");
		CHECK(TabSpace.ModifyString(L"") == L"");
	}

	void RunCommaPerDotTrim()
	{
		CommaPerDotTrim CommaToDot;

		CHECK(CommaToDot.ModifyString(L"12,5") == L"12.5");
		CHECK(CommaToDot.ModifyString(L"12.5") == L"12.5");
		CHECK(CommaToDot.ModifyString(L"  12,5\r\n") == L"12.5");
		CHECK(CommaToDot.ModifyString(L"1,5\t2,25") == L"1.5\t2.25");
		CHECK(CommaToDot.ModifyString(L" ") == L"");
	}

	void RunDotPerComma()
	{
		DotPerComma DotToComma;

		CHECK(DotToComma.ModifyString(L"12.5") == L"12,5");
		CHECK(DotToComma.ModifyString(L"12,5") == L"12,5");
		CHECK(DotToComma.ModifyString(L"  12.5\r\n") == L"12,5");
		CHECK(DotToComma.ModifyString(L"1.5\t2.25") == L"1,5\t2,25");
	}

	void RunRemoveDotsCommaPerDot()
	{
		RemoveDotsCommaPerDot Thousands;

		CHECK(Thousands.ModifyString(L"7.584,93") == L"7584.93");
		CHECK(Thousands.ModifyString(L"1.234.567,8") == L"1234567.8");
		CHECK(Thousands.ModifyString(L"12,5") == L"12.5");
		CHECK(Thousands.ModifyString(L" 7.584,93\r\n") == L"7584.93");
		// Without a comma the text is left untouched
		CHECK(Thousands.ModifyString(L"7.584") == L"7.584");
		CHECK(Thousands.ModifyString(L"Hola") == L"Hola");
	}

	// Returns true when all checks pass
	bool Run()
	{
		RunPaperTittleCamelCase();
		RunUpperCase();
		RunLowerCase();
		RunParagraphCase();
		RunEmptyString();
		RunDontModify();
		RunChangeSpaceTab();
		RunChangeTabSpace();
		RunCommaPerDotTrim();
		RunDotPerComma();
		RunRemoveDotsCommaPerDot();

		std::cout << m_Checks - m_Failures << "/" << m_Checks << " checks passed" << std::endl;
		return m_Failures == 0;
	}
};
