#pragma once
#include "PaperTittleCamelCase.h"
#include "ParagraphCase.h"
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

	void Run()
	{
		RunPaperTittleCamelCase();
		RunUpperCase();
		RunLowerCase();
		RunParagraphCase();
	}
};
