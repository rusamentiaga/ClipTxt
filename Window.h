#pragma once
#include <windows.h>
#include <iostream>
#include <vector>

namespace AC 
{
	class Window
	{
	protected:
		HWND m_hWnd;
		WNDPROC m_pPrevWndProc;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
											WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnMessage(UINT message, WPARAM wParam, LPARAM lParam);

		typedef LRESULT (Window::*MsgHandlerFunc)(WPARAM wParam, LPARAM lParam);
		struct MsgHandlerEntry
		{
			UINT nMessage;			// windows message
			MsgHandlerFunc pfn;		// routine to call (or special value)

			int nLoWParam; // -1 = *
			int nHiWParam; // -1 = *
			int nLoLParam; // -1 = *
			int nHiLParam; // -1 = *
		};
		std::vector<MsgHandlerEntry> m_MsgHandlers;
		MsgHandlerFunc FindMessageHandler(UINT message, WPARAM wParam, LPARAM lParam);

		void AddMessageHandler(UINT nMessage, MsgHandlerFunc pfn, 
						int nLoWParam = AnyValue, int nHiWParam = AnyValue, 
						int nLoLParam = AnyValue, int nHiLParam = AnyValue);

		static const int AnyValue = -1;
	public:
		Window();
		virtual ~Window();

		BOOL DestroyWindow();

		void RegisterClass(HINSTANCE hInst, LPCTSTR lpszClassName, UINT nIDMenu = NULL,
							DWORD dwStyle = CS_HREDRAW | CS_VREDRAW, 
							HCURSOR hCursor = NULL, HBRUSH hbrBg = NULL,
							HICON hIcon = NULL);

		void Create(HINSTANCE hInst, LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
					DWORD dwStyle = WS_OVERLAPPEDWINDOW, int x = CW_USEDEFAULT,
					int y = 0, int nWidth = CW_USEDEFAULT,
					int nHeight = 0, HWND pParentWnd = NULL, 
					UINT nID = 0);

		inline operator HWND(void) const {return m_hWnd;}
	};
}
