#pragma once
#include <shellapi.h>
#include "Window.h"
#include "SharedPtr.h"
#include <map>
#include <comdef.h> // _com_error
#include <chrono>

#include "PaperTittleCamelCase.h"
#include "ParagraphCase.h"
#include "ChangeCase.h"
#include "SpaceTab.h"
#include "DecimalSeparator.h"

class MainWindow : public AC::Window
{
protected:
	static const int WM_TRAYICON_NOTIFICATION = (WM_APP + 1);
	static const int DontModifyStrategyIndex = 0;

	HWND m_hWndNextViewer;

	typedef SharedPtr<ClipboardModifyStrategy> ClipboardModifyStrategyPtr;
	ClipboardModifyStrategyPtr m_SelectedModifyStrategy;
	std::map<int, ClipboardModifyStrategyPtr> m_AvailableStrategies;

	BOOL m_bEnabled;
	BOOL m_bVerbose;
	NOTIFYICONDATA m_TrayIconData;

	static const int MAX_LOADSTRING = 100;
	TCHAR m_szTitle[MAX_LOADSTRING];
	TCHAR m_szWindowClass[MAX_LOADSTRING];

	std::chrono::steady_clock::time_point g_lastClipboardSetTime;
	const int IGNORE_DURATION_MS = 200;  

	virtual LRESULT OnWmCreate(WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
	    // Add the window to the clipboard viewer chain. 
	    //m_hWndNextViewer = SetClipboardViewer(m_hWnd); 

		AddClipboardFormatListener(m_hWnd);

		memset(&m_TrayIconData, 0 , sizeof(m_TrayIconData));
		m_TrayIconData.cbSize = sizeof(m_TrayIconData);
		m_TrayIconData.uID = IDR_TRAYICON_MENU;
		m_TrayIconData.hWnd = m_hWnd;
		m_TrayIconData.uCallbackMessage = WM_TRAYICON_NOTIFICATION;
		m_TrayIconData.hIcon = LoadIcon(GetModuleHandle(NULL), (LPCTSTR)IDI_CLIPTXT);
		lstrcpy(m_TrayIconData.szTip, m_szTitle);
		m_TrayIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;

		Shell_NotifyIcon(NIM_ADD , &m_TrayIconData);

		return 0;
	}

	virtual LRESULT OnWmDestroy(WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
		Shell_NotifyIcon(NIM_DELETE , &m_TrayIconData);
		ChangeClipboardChain(m_hWnd, m_hWndNextViewer); 
		PostQuitMessage(0);
		return 0;
	}

	virtual LRESULT OnExit(WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
		DestroyWindow();
		return 0;
	}

	virtual LRESULT OnChangeCbChain(WPARAM wParam, LPARAM lParam)
	{
		// If the next window is closing, repair the chain. 
		if ((HWND) wParam == m_hWndNextViewer) 
			m_hWndNextViewer = (HWND) lParam; 
		// Otherwise, pass the message to the next link. 
		else if (m_hWndNextViewer != NULL) 
			SendMessage(m_hWndNextViewer, WM_CHANGECBCHAIN, wParam, lParam); 

		return 0;
	}

	bool TryOpenClipboard(HWND hwndOwner, int retries = 5, int delayMs = 50)
	{
		for (int i = 0; i < retries; ++i)
		{
			if (OpenClipboard(hwndOwner))
				return true;
			Sleep(delayMs);
		}
		return false;
	}

	void ShowError(TCHAR* Title)
	{
		_com_error error(GetLastError());
		LPCTSTR errorText = error.ErrorMessage();

		m_TrayIconData.uFlags = NIF_INFO;
		_tcscpy_s(m_TrayIconData.szInfo, _countof(m_TrayIconData.szInfo), errorText);
		_tcscpy_s(m_TrayIconData.szInfoTitle, _countof(m_TrayIconData.szInfoTitle), Title);
		m_TrayIconData.uTimeout = 5000;
		m_TrayIconData.dwInfoFlags = NIIF_ERROR;
		Shell_NotifyIcon(NIM_MODIFY, &m_TrayIconData);
	}

	void ShowMessage(const std::wstring& ClipText, const std::wstring& Title)
	{
		m_TrayIconData.uFlags = NIF_INFO;
		_tcscpy_s(m_TrayIconData.szInfo, _countof(m_TrayIconData.szInfo), ClipText.c_str());
		_tcscpy_s(m_TrayIconData.szInfoTitle, _countof(m_TrayIconData.szInfoTitle), Title.c_str());
		m_TrayIconData.uTimeout = 1000;
		m_TrayIconData.dwInfoFlags = NIIF_INFO;
		Shell_NotifyIcon(NIM_MODIFY, &m_TrayIconData);
	}

	virtual LRESULT OnDrawClipboard(WPARAM wParam, LPARAM lParam)
	{
		auto now = std::chrono::steady_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			now - g_lastClipboardSetTime
		).count();

		if (elapsed_ms < IGNORE_DURATION_MS)
			return 0;

		bool btransformed = false;
		std::wstring ClipText;

		//SendMessage(m_hWndNextViewer, WM_DRAWCLIPBOARD, wParam, lParam); 

		if ((!m_bEnabled) || (m_SelectedModifyStrategy == NULL))
		{
			return 0;
		}

		if (TryOpenClipboard(NULL))
        { 
			HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);
			if (!hglb)
			{
				CloseClipboard();

#ifdef _DEBUG
				_tprintf(L"Entro sin texto\n");
#endif
				return 0;
			}

            LPTSTR lpstr = (LPTSTR)GlobalLock(hglb); 

			if (lpstr)
				ClipText = lpstr;

			GlobalUnlock(hglb); 

#ifdef _DEBUG
			_tprintf(L"Entro con %s \n", ClipText.c_str());
#endif

			if (!ClipText.empty())
			{
				ClipText = m_SelectedModifyStrategy->ModifyString(ClipText);

				EmptyClipboard();

				hglb = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, (ClipText.length() + 1)*sizeof(WCHAR));
				if (hglb == NULL)
				{
					CloseClipboard();
					return 0;
				}
				lpstr = (LPTSTR)GlobalLock(hglb); 

				lstrcpy(lpstr, ClipText.c_str());

				GlobalUnlock(hglb);
				SetClipboardData(CF_UNICODETEXT, hglb);

				g_lastClipboardSetTime = std::chrono::steady_clock::now();

#ifdef _DEBUG
				_tprintf(L" Salgo con %s\n", ClipText.c_str());
#endif

				btransformed = true;
			}
			CloseClipboard();
		}
		else
		{
			if (m_bVerbose)
			{
				ShowError(_T("Error in OpenClipboard"));
			}
		}

		if (btransformed && m_bVerbose)
		{
			ShowMessage(ClipText, _T("Text transformed"));
		}

		return 0;
	}

	LRESULT OnTrayIconNotification(WPARAM wParam, LPARAM lParam)
	{
		if (wParam != m_TrayIconData.uID || (lParam != WM_RBUTTONUP && lParam != WM_LBUTTONDBLCLK))
			return 0;

		HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(m_TrayIconData.uID));
		if (!hMenu)
			return 0;
		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (!hSubMenu)
			return 0;

		std::map<int, ClipboardModifyStrategyPtr>::iterator iter;
		for (iter = m_AvailableStrategies.begin(); iter != m_AvailableStrategies.end(); iter++)
		{
			if ((m_SelectedModifyStrategy == iter->second) && (iter->first != DontModifyStrategyIndex))
				break;
		}
		if (iter != m_AvailableStrategies.end())
			CheckMenuItem(hSubMenu, iter->first, MF_BYCOMMAND|MF_CHECKED );

		if (!m_bEnabled)
		{
			for (iter = m_AvailableStrategies.begin(); iter != m_AvailableStrategies.end(); iter++)
				EnableMenuItem(hSubMenu, iter->first, MF_DISABLED|MF_GRAYED);

			EnableMenuItem(hSubMenu, ID_MENU_TITTECAMELCASE, MF_DISABLED|MF_GRAYED);
			EnableMenuItem(hSubMenu, ID_MENU_UPPERCASE, MF_DISABLED|MF_GRAYED);
			EnableMenuItem(hSubMenu, ID_MENU_LOWERCASE, MF_DISABLED|MF_GRAYED);
			EnableMenuItem(hSubMenu, ID_MENU_PARAGRAPHCASE, MF_DISABLED|MF_GRAYED);
			CheckMenuItem(hSubMenu, ID_MENU_DISABLE, MF_BYCOMMAND|MF_CHECKED );
		}

		if (m_bVerbose)
		{
			CheckMenuItem(hSubMenu, ID_MENU_VERBOSE, MF_BYCOMMAND | MF_CHECKED);
		}

		if (lParam == WM_RBUTTONUP) 
		{
			// Make first menu item the default (bold font)
			//::SetMenuDefaultItem(hSubMenu, 0, TRUE);

			// Display the menu at the current mouse location. There's a "bug"
			// (Microsoft calls it a feature) in Windows 95 that requires calling
			// SetForegroundWindow. To find out more, search for Q135788 in MSDN.
			//
			POINT MousePoint;
			GetCursorPos(&MousePoint);
			SetForegroundWindow(m_TrayIconData.hWnd);	
			TrackPopupMenu(hSubMenu, 0, MousePoint.x, MousePoint.y, 0,m_TrayIconData.hWnd, NULL);
		} 
		else
			::SendMessage(m_TrayIconData.hWnd, WM_COMMAND, GetMenuItemID(hSubMenu,0), 0);

		return 1;
	}

	virtual LRESULT OnDisable(WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
		m_bEnabled = !m_bEnabled;
		return 0;
	}

	virtual LRESULT OnVerbose(WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
		m_bVerbose = !m_bVerbose;
		return 0;
	}

	virtual LRESULT OnChangeStrategy(WPARAM wParam, LPARAM /*lParam*/)
	{
		int StrategyIndex = LOWORD(wParam);

		if (m_SelectedModifyStrategy == m_AvailableStrategies[StrategyIndex])
			m_SelectedModifyStrategy = m_AvailableStrategies[DontModifyStrategyIndex];
		else
			m_SelectedModifyStrategy = m_AvailableStrategies[StrategyIndex];
		return 0;
	}


public:
	MainWindow(void)
	{
		AddMessageHandler(WM_CREATE, (MsgHandlerFunc)&MainWindow::OnWmCreate);
		AddMessageHandler(WM_DESTROY, (MsgHandlerFunc)&MainWindow::OnWmDestroy);
		AddMessageHandler(WM_COMMAND, (MsgHandlerFunc)&MainWindow::OnExit, IDM_EXIT, BN_CLICKED);
		AddMessageHandler(WM_TRAYICON_NOTIFICATION, (MsgHandlerFunc)&MainWindow::OnTrayIconNotification);

		//AddMessageHandler(WM_CHANGECBCHAIN, (MsgHandlerFunc)&MainWindow::OnChangeCbChain);
		//AddMessageHandler(WM_DRAWCLIPBOARD, (MsgHandlerFunc)&MainWindow::OnDrawClipboard);

		AddMessageHandler(WM_CLIPBOARDUPDATE, (MsgHandlerFunc)&MainWindow::OnDrawClipboard);
		

		AddMessageHandler(WM_COMMAND, (MsgHandlerFunc)&MainWindow::OnChangeStrategy, ID_MENU_LOWERCASE, BN_CLICKED);
		AddMessageHandler(WM_COMMAND, (MsgHandlerFunc)&MainWindow::OnChangeStrategy, ID_MENU_UPPERCASE, BN_CLICKED);
		AddMessageHandler(WM_COMMAND, (MsgHandlerFunc)&MainWindow::OnChangeStrategy, ID_MENU_TITTECAMELCASE, BN_CLICKED);
		AddMessageHandler(WM_COMMAND, (MsgHandlerFunc)&MainWindow::OnChangeStrategy, ID_MENU_PARAGRAPHCASE, BN_CLICKED);

		AddMessageHandler(WM_COMMAND, (MsgHandlerFunc)&MainWindow::OnDisable, ID_MENU_DISABLE, BN_CLICKED);
		AddMessageHandler(WM_COMMAND, (MsgHandlerFunc)&MainWindow::OnVerbose, ID_MENU_VERBOSE, BN_CLICKED);
	
		m_AvailableStrategies[ID_MENU_LOWERCASE] = new LowerCase;
		m_AvailableStrategies[ID_MENU_UPPERCASE] = new UpperCase;
		m_AvailableStrategies[ID_MENU_TITTECAMELCASE] = new PaperTittleCamelCase;
		m_AvailableStrategies[ID_MENU_PARAGRAPHCASE] = new ParagraphCase;
		m_AvailableStrategies[ID_MENU_SPACETAB] = new ChangeSpaceTab;
		m_AvailableStrategies[ID_MENU_TABSPACE] = new ChangeTabSpace;
		m_AvailableStrategies[ID_MENU_DOTPERCOMMA] = new CommaPerDotTrim;
		m_AvailableStrategies[ID_MENU_COMMAPERDOT] = new DotPerComma;
		m_AvailableStrategies[ID_MENU_REMOVEDOTSCOMMAPERDOT] = new RemoveDotsCommaPerDot;

		std::map<int, ClipboardModifyStrategyPtr>::iterator iter;
		for (iter = m_AvailableStrategies.begin(); iter != m_AvailableStrategies.end(); iter++)
			AddMessageHandler(WM_COMMAND, (MsgHandlerFunc)&MainWindow::OnChangeStrategy, iter->first, BN_CLICKED);

		m_AvailableStrategies[DontModifyStrategyIndex] = new DontModify;
		m_SelectedModifyStrategy = m_AvailableStrategies[DontModifyStrategyIndex];
		m_bEnabled = TRUE;
		m_bVerbose = FALSE;
	}

	virtual ~MainWindow(void)
	{
	}

public:
	void Create()
	{
		LoadString(GetModuleHandle(NULL), IDS_APP_TITLE, m_szTitle, MAX_LOADSTRING);
		LoadString(GetModuleHandle(NULL), IDC_CLIPTXT, m_szWindowClass, MAX_LOADSTRING);

		RegisterClass(GetModuleHandle(NULL), m_szWindowClass, IDC_CLIPTXT);
		Window::Create(GetModuleHandle(NULL), m_szWindowClass, m_szTitle);

#ifdef _DEBUG
		AllocConsole();
		freopen ("CONOUT$", "w", stdout ); 
#endif
	}
};
