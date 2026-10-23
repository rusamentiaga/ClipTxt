#pragma once
#include "PaperTittleCamelCase.h"
#include "ParagraphCase.h"
#include "ChangeCase.h"
#include "SpaceTab.h"
#include "DecimalSeparator.h"
#include <assert.h>


#include <iostream>
#include <locale>
#include <string> 

class UnitTest
{
public:

	UnitTest(void)
	{
	}

	~UnitTest(void)
	{
	}


	void RunPaperTittleCamelCase()
	{
		PaperTittleCamelCase CamelCase;

		assert(CamelCase.ModifyString(L"Hola") == L"Hola");
		assert(CamelCase.ModifyString(L"Hola2") == L"Hola2");
		assert(CamelCase.ModifyString(L"HOLA") == L"Hola");		
		assert(CamelCase.ModifyString(L"HOLA HOLA") == L"Hola Hola");		
		assert(CamelCase.ModifyString(L"HOLA de HOLA") == L"Hola de Hola");	
		assert(CamelCase.ModifyString(L"HOLA PARA HOLA") == L"Hola para Hola");		
		assert(CamelCase.ModifyString(L"HOLA LAS HOLA") == L"Hola las Hola");		
		assert(CamelCase.ModifyString(L"HOLA THE HOLA") == L"Hola the Hola");		
		assert(CamelCase.ModifyString(L"THE HOLA THE HOLA") == L"The Hola the Hola");		
		assert(CamelCase.ModifyString(L"THE HOLA in HOLA") == L"The Hola in Hola");		
		assert(CamelCase.ModifyString(L"ALUMNOS DE INGENIERÍA TÉCNICA INFORMÁTICA") == L"Alumnos de Ingeniería Técnica Informática");
		assert(CamelCase.ModifyString(L"la INFORMÁTICA") == L"La Informática");
	}

	void RunUpperCase()
	{
		UpperCase UpCase;

		assert(UpCase.ModifyString(L"Hola") == L"HOLA");
		assert(UpCase.ModifyString(L"HOLA") == L"HOLA");
		assert(UpCase.ModifyString(L"hola") == L"HOLA");
	}

	void RunLowerCase()
	{
		LowerCase LowCase;

		assert(LowCase.ModifyString(L"Hola") == L"hola");
		assert(LowCase.ModifyString(L"HOLA") == L"hola");
		assert(LowCase.ModifyString(L"hola") == L"hola");
	}

	void RunParagraphCase()
	{
		ParagraphCase ParCase;

		assert(ParCase.ModifyString(L"Hola") == L"Hola");
		assert(ParCase.ModifyString(L"HOLA que tal") == L"Hola que tal");
		assert(ParCase.ModifyString(L"Hola. ADIOS") == L"Hola. Adios");
		assert(ParCase.ModifyString(L"Hola. ADIOS\n\nhasta luego") == L"Hola. Adios\n\nHasta luego");
		assert(ParCase.ModifyString(L"Hola. ADIOS\n\nítaca") == L"Hola. Adios\n\nÍtaca");
	}

	void RunEmptyString()
	{
		assert(PaperTittleCamelCase().ModifyString(L"") == L"");
		assert(ParagraphCase().ModifyString(L"") == L"");
		assert(UpperCase().ModifyString(L"") == L"");
		assert(LowerCase().ModifyString(L"") == L"");
	}

	void RunDontModify()
	{
		DontModify NoChange;

		// Default strategy: text is only re-copied as plain text, unchanged
		assert(NoChange.ModifyString(L"Hola") == L"Hola");
		assert(NoChange.ModifyString(L"  Hola\tque\r\ntal  ") == L"  Hola\tque\r\ntal  ");
		assert(NoChange.ModifyString(L"") == L"");
	}

	void RunChangeSpaceTab()
	{
		ChangeSpaceTab SpaceTab;

		assert(SpaceTab.ModifyString(L"a b c") == L"a\tb\tc");
		assert(SpaceTab.ModifyString(L"1  2") == L"1\t\t2");
		assert(SpaceTab.ModifyString(L"abc") == L"abc");
		assert(SpaceTab.ModifyString(L" a b ") == L"a\tb");
	}

	void RunChangeTabSpace()
	{
		ChangeTabSpace TabSpace;

		assert(TabSpace.ModifyString(L"a	b	c") == L"a   b   c");
		assert(TabSpace.ModifyString(L"1		2") == L"1      2");
		assert(TabSpace.ModifyString(L"abc") == L"abc");
		assert(TabSpace.ModifyString(L"a b") == L"a b");
		assert(TabSpace.ModifyString(L"	a	b	") == L"a   b");
		assert(TabSpace.ModifyString(L"") == L"");
	}

	void RunCommaPerDotTrim()
	{
		CommaPerDotTrim CommaToDot;

		assert(CommaToDot.ModifyString(L"12,5") == L"12.5");
		assert(CommaToDot.ModifyString(L"12.5") == L"12.5");
		assert(CommaToDot.ModifyString(L"  12,5\r\n") == L"12.5");
		assert(CommaToDot.ModifyString(L"1,5\t2,25") == L"1.5\t2.25");
		assert(CommaToDot.ModifyString(L" ") == L"");
	}

	void RunDotPerComma()
	{
		DotPerComma DotToComma;

		assert(DotToComma.ModifyString(L"12.5") == L"12,5");
		assert(DotToComma.ModifyString(L"12,5") == L"12,5");
		assert(DotToComma.ModifyString(L"  12.5\r\n") == L"12,5");
		assert(DotToComma.ModifyString(L"1.5\t2.25") == L"1,5\t2,25");
	}

	void RunRemoveDotsCommaPerDot()
	{
		RemoveDotsCommaPerDot Thousands;

		assert(Thousands.ModifyString(L"7.584,93") == L"7584.93");
		assert(Thousands.ModifyString(L"1.234.567,8") == L"1234567.8");
		assert(Thousands.ModifyString(L"12,5") == L"12.5");
		assert(Thousands.ModifyString(L" 7.584,93\r\n") == L"7584.93");
		// Without a comma the text is left untouched
		assert(Thousands.ModifyString(L"7.584") == L"7.584");
		assert(Thousands.ModifyString(L"Hola") == L"Hola");
	}

	void Run()
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
	}
};
