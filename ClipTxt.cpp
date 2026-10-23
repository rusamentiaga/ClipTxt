// ClipTxt.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ClipTxt.h"
#include "MainWindow.h"

#include "tstring.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Construct locale object with the system's setting
	std::locale loc("");
	std::locale::global(loc); 

	try
	{
		MainWindow Window;
		Window.Create();

		ShowWindow(Window, SW_HIDE);
		UpdateWindow(Window);

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) 
		{
			TranslateMessage(&msg);

			try
			{
				DispatchMessage(&msg);
			}
			catch(std::exception& ex)
			{
				AC::tstring Reason = AnsiTotstring(ex.what());
				MessageBox(Window, Reason.c_str(), _T("Application error"), MB_OK|MB_ICONERROR);
			}
		}
		return (int)msg.wParam;
	}
	catch(std::exception& ex)
	{
		AC::tstring Reason = AnsiTotstring(ex.what());
		MessageBox(NULL, Reason.c_str(), _T("Application failure"), MB_OK|MB_ICONERROR);
		return EXIT_FAILURE;
	}
	return 0;
}
