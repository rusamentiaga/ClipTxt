#include "stdafx.h"
#include "Window.h"
#include "Exception.h"
#include "Util.h"

namespace AC 
{
	Window::Window()
	{
		m_hWnd = NULL;
		m_pPrevWndProc = NULL;
	}

	Window::~Window()
	{
		if (m_hWnd != NULL)
		{
			DestroyWindow();
			m_hWnd = NULL;
		}
	}

	BOOL Window::DestroyWindow()
	{
		BOOL bResult = TRUE;

		if (m_hWnd != NULL)
			bResult = ::DestroyWindow(m_hWnd);

		m_hWnd = NULL;
		m_pPrevWndProc = NULL;

		return bResult;
	}

	void Window::RegisterClass(HINSTANCE hInst, LPCTSTR lpszClassName, UINT nIDMenu,
										   DWORD dwStyle, HCURSOR hCursor, 
										   HBRUSH hbrBackground, HICON hIcon)
	{
		WNDCLASSEX wcx;

		wcx.cbSize = sizeof(wcx);  
		wcx.style = dwStyle;                   
		wcx.lpfnWndProc = WndProc;     
		wcx.cbClsExtra = 0;               
		wcx.cbWndExtra = 0;               
		wcx.hInstance = hInst;  

		if (hIcon == NULL)
			wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);       
		else
			wcx.hIcon = hIcon;

		if (hCursor == NULL)
			wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
		else
			wcx.hCursor = hCursor;

		if (hbrBackground == NULL)
			wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
		else
			wcx.hbrBackground = hbrBackground; 

		if (nIDMenu == NULL)
			wcx.lpszMenuName =  NULL;    
		else
			wcx.lpszMenuName =  MAKEINTRESOURCE(nIDMenu);    

		wcx.lpszClassName = lpszClassName;
		wcx.hIconSm = NULL;
	 
		if (!RegisterClassEx(&wcx))
			if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
				throw Exception() << Util::StrError(GetLastError());
	}

	void Window::Create(HINSTANCE hInst, LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
								DWORD dwStyle, int x, int y, int nWidth, int nHeight,
								HWND pParentWnd, UINT nID)
	{
		m_hWnd = CreateWindow(lpszClassName, lpszWindowName, dwStyle,
			x, y, nWidth, nHeight, pParentWnd, (HMENU)(UINT_PTR)nID, hInst, this);

		if (m_hWnd == NULL)
			throw Exception() << Util::StrError(GetLastError());

		// Subclassing
		WNDCLASSEX wcx;
		if (GetClassInfoEx(hInst, lpszClassName, &wcx))
		{
			if (wcx.lpfnWndProc != WndProc)
			{
				m_pPrevWndProc = wcx.lpfnWndProc;
				SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG)(UINT_PTR)WndProc);
				SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG)(UINT_PTR)this);
			}
		}
	}

	LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Window *self;

		if (uMsg == WM_NCCREATE) 
		{
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			self = reinterpret_cast<Window *>(lpcs->lpCreateParams);
			self->m_hWnd = hWnd;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)self);
		} 
		else 
			self = (Window*)(UINT_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		if (self) 
			return self->OnMessage(uMsg, wParam, lParam);
		else 
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	Window::MsgHandlerFunc Window::FindMessageHandler(UINT message, WPARAM wParam, LPARAM lParam)
	{
		int nLoWParam = LOWORD(wParam);
		int nHiWParam = HIWORD(wParam);
		int nLoLParam = LOWORD(lParam);
		int nHiLParam = HIWORD(lParam);

		for (unsigned int i = 0; i < m_MsgHandlers.size(); i++)
		{
			if (m_MsgHandlers[i].nMessage == message)
				if ((m_MsgHandlers[i].nLoWParam == AnyValue) || (m_MsgHandlers[i].nLoWParam == nLoWParam))
					if ((m_MsgHandlers[i].nHiWParam == AnyValue) || (m_MsgHandlers[i].nHiWParam == nHiWParam))
						if ((m_MsgHandlers[i].nLoLParam == AnyValue) || (m_MsgHandlers[i].nLoLParam == nLoLParam))
							if ((m_MsgHandlers[i].nHiLParam == AnyValue) || (m_MsgHandlers[i].nHiLParam == nHiLParam))
								return m_MsgHandlers[i].pfn;
		}
		return NULL;
	}

	LRESULT Window::OnMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		MsgHandlerFunc pfn = FindMessageHandler(message, wParam, lParam);

		// Subclassing
		if (m_pPrevWndProc)
		{
			if (pfn)
				(this->*pfn)(wParam, lParam);
			return CallWindowProc(m_pPrevWndProc, m_hWnd, message, wParam, lParam);
		}

		if (pfn)
			return (this->*pfn)(wParam, lParam);
		else
			return DefWindowProc(m_hWnd, message, wParam, lParam);
	}

	void Window::AddMessageHandler(UINT nMessage, MsgHandlerFunc pfn,
						int nLoWParam, int nHiWParam, 
						int nLoLParam, int nHiLParam)
	{
		Window::MsgHandlerEntry Entry = {
			nMessage, pfn, nLoWParam, nHiWParam, nLoLParam, nHiLParam };
		m_MsgHandlers.push_back(Entry);
	}
}
