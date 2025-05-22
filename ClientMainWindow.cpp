#include "StdAfx.h"
#include "ClientMainWindow.h"
#include "ConnectDialog.h"

#define MAX_LOADSTRING 100

#define INTRO_ID_TIMER 100
#define INTRO_PERIOD 10

ClientMainWindow::ClientMainWindow(void)
{
	AddMessageHandler(WM_CREATE,	
		(MsgHandlerFunc)&ClientMainWindow::OnWmCreate);
	AddMessageHandler(WM_DESTROY,	
		(MsgHandlerFunc)&ClientMainWindow::OnWmDestroy);
	AddMessageHandler(WM_ERASEBKGND,
		(MsgHandlerFunc)&ClientMainWindow::OnEraseBkgnd);
	AddMessageHandler(WM_PAINT,		
		(MsgHandlerFunc)&ClientMainWindow::OnPaint);
	AddMessageHandler(WM_TIMER,		
		(MsgHandlerFunc)&ClientMainWindow::OnTimer);
	AddMessageHandler(WM_KEYDOWN,		
		(MsgHandlerFunc)&ClientMainWindow::OnKeyDown);
	AddMessageHandler(WM_KEYUP,		
		(MsgHandlerFunc)&ClientMainWindow::OnKeyUp);

	AddMessageHandler(WM_COMMAND, 
		(MsgHandlerFunc)&ClientMainWindow::OnExit, IDM_EXIT, BN_CLICKED);
	AddMessageHandler(WM_COMMAND, 
		(MsgHandlerFunc)&ClientMainWindow::OnFileConnect, ID_FILE_CONNECT, BN_CLICKED);

	AddMessageHandler(WM_SOCKET, 
		(MsgHandlerFunc)&ClientMainWindow::OnFdConnect, AC::Window::AnyValue,
		AC::Window::AnyValue, FD_CONNECT, AC::Window::AnyValue);
	AddMessageHandler(WM_SOCKET, 
		(MsgHandlerFunc)&ClientMainWindow::OnFdRead, AC::Window::AnyValue,
		AC::Window::AnyValue, FD_READ, AC::Window::AnyValue);
	AddMessageHandler(WM_SOCKET, 
		(MsgHandlerFunc)&ClientMainWindow::OnFdClose, AC::Window::AnyValue,
		AC::Window::AnyValue, FD_CLOSE, AC::Window::AnyValue);

	m_bConnected = FALSE;
	m_MapKeyToDir[VK_UP] = MSG_CLIENT_MOVE_NORTH;
	m_MapKeyToDir[VK_DOWN] = MSG_CLIENT_MOVE_SOUTH;
	m_MapKeyToDir[VK_LEFT] = MSG_CLIENT_MOVE_EAST;
	m_MapKeyToDir[VK_RIGHT] = MSG_CLIENT_MOVE_WEST;
	m_MapKeyToDir[VK_SPACE] = MSG_CLIENT_FIRE;
}

ClientMainWindow::~ClientMainWindow(void)
{
}

void ClientMainWindow::Create()
{
	TCHAR szTitle[MAX_LOADSTRING];
	TCHAR szWindowClass[MAX_LOADSTRING];

	LoadString(GetModuleHandle(NULL), IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(GetModuleHandle(NULL), IDC_KURSKBATTLECLIENT, szWindowClass, MAX_LOADSTRING);

	RegisterClass(GetModuleHandle(NULL), szWindowClass, IDC_KURSKBATTLECLIENT);
	Window::Create(GetModuleHandle(NULL), szWindowClass, szTitle);
}

BOOL ClientMainWindow::SetClientRect(RECT& NewClientRect)
{
	RECT ClientRect;
	BOOL bMenu;

	bMenu = (GetMenu(*this) ? TRUE :  FALSE);

	AdjustWindowRectEx(&NewClientRect, GetWindowLong(*this,GWL_STYLE), bMenu, 
		GetWindowLong(*this, GWL_EXSTYLE));

	GetWindowRect(*this, &ClientRect);

	return MoveWindow(*this, ClientRect.left, ClientRect.top, 
		NewClientRect.right - NewClientRect.left, 
		NewClientRect.bottom - NewClientRect.top, TRUE);
}

LRESULT ClientMainWindow::OnWmCreate(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (!m_ScreenManager.LoadSprites())
		return -1;

	RECT GameClientRect = {0, 0, GAME_SURFACE_WIDTH, GAME_SURFACE_HEIGHT};
	SetClientRect(GameClientRect);

	m_Intro.Start(m_GameStatus);
	SetTimer(*this, INTRO_ID_TIMER, INTRO_PERIOD, NULL);

#ifdef _DEBUG
	AllocConsole();
	FILE *stream;
	freopen_s (&stream, "CONOUT$", "w", stdout ); 
#endif

	return 0;
}

LRESULT ClientMainWindow::OnWmDestroy(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	PostQuitMessage(0);
	return 0;
}

LRESULT ClientMainWindow::OnEraseBkgnd(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return 0;
}

LRESULT ClientMainWindow::OnPaint(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	PAINTSTRUCT ps;

	HDC hdc = BeginPaint(*this, &ps);
	m_ScreenManager.Update(hdc, m_GameStatus);
	EndPaint(*this, &ps);

	return 0;
}

LRESULT ClientMainWindow::OnTimer(WPARAM wParam, LPARAM /*lParam*/)
{
	if (wParam == INTRO_ID_TIMER)
	{
		m_Intro.Update(m_GameStatus);
		InvalidateRect(*this, NULL, TRUE);
	}

	return 0;
}

void ClientMainWindow::SendPlayerInfo(const PlayerMsg& Msg)
{
	try
	{
		m_TcpClient.Send(Msg);
	}
	catch(std::exception& ex)
	{
		AC::tstring Reason = AnsiTotstring(ex.what());
		MessageBox(*this, Reason.c_str(), _T("Connection broken"), MB_OK|MB_ICONERROR);
		StopGame();
	}
}

LRESULT ClientMainWindow::OnKeyDown(WPARAM wParam, LPARAM /*lParam*/)
{
	if (m_bConnected)
	{
		std::map<int, int>::const_iterator iter;

		iter = m_MapKeyToDir.find(wParam);
		if (iter != m_MapKeyToDir.end())
		{
			if (MSG_CLIENT_FIRE != iter->second)
			{
				m_LastTankDir = iter->second;
				SendPlayerInfo(m_LastTankDir);
			}
		}
	}

	return 0;
}

LRESULT ClientMainWindow::OnKeyUp(WPARAM wParam, LPARAM /*lParam*/)
{
	if (m_bConnected)
	{
		std::map<int, int>::const_iterator iter;

		iter = m_MapKeyToDir.find(wParam);
		if (iter != m_MapKeyToDir.end())
		{
			BOOL CanShoot = (m_ReloadingTimer.Milliseconds() > SHOOT_INTERVAL);
			if ((MSG_CLIENT_FIRE == iter->second) && CanShoot)
			{
				SendPlayerInfo(MSG_CLIENT_FIRE);
				m_ReloadingTimer.Start();
			}
			else
			{
				if (m_LastTankDir == iter->second)
				{
					m_LastTankDir = MSG_CLIENT_MOVE_NONE;
					SendPlayerInfo(m_LastTankDir);
				}
			}
		}
	}
	return 0;
}

LRESULT ClientMainWindow::OnExit(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	DestroyWindow();
	return 0;
}

LRESULT ClientMainWindow::OnFileConnect(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	try
	{
		ConnectDialog ConDialog;

		if (ConDialog.DoModal(*this, GetModuleHandle(NULL)) == IDOK)
		{
			KillTimer(*this, INTRO_ID_TIMER);

			m_pReadStateFun = &ClientMainWindow::OnFdRead_PlayerNum;

			m_Nickname = ConDialog.GetNickname();
			AC::InetAddress ServerAddress(ConDialog.GetAddress(), ConDialog.GetPort());
			m_TcpClient.AsyncSelect(*this, WM_SOCKET, FD_CONNECT | FD_READ | FD_CLOSE);
			m_TcpClient.Connect(ServerAddress);
		}
	}
	catch(std::exception& ex)
	{
		AC::tstring Reason = AnsiTotstring(ex.what());
		MessageBox(*this, Reason.c_str(), _T("Error connecting"), MB_OK|MB_ICONERROR);
		StopGame();
	}

	return 0;
}

LRESULT ClientMainWindow::OnFdConnect(WPARAM /*wParam*/, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		MessageBox(*this, _T("Error connecting"), _T("Error"), MB_OK|MB_ICONERROR);
		StopGame();
	}
	else
	{
		TCHAR Nickname[MAX_PLAYER_NAME];
		lstrcpyn(Nickname, m_Nickname.c_str(), MAX_PLAYER_NAME);
		m_TcpClient.Send(Nickname);

		m_TcpClient.DisableNagle();
		m_bConnected = TRUE;
	}

	return 0;
}

LRESULT ClientMainWindow::OnFdRead(WPARAM /*wParam*/, LPARAM lParam)
{
	try
	{
		if (m_bConnected)
			(this->*m_pReadStateFun)();
	}
	catch(std::exception& ex)
	{
		printf("Expception\n");
		StopGame();
		m_GameStatus.Clear();
		AC::tstring Reason = AnsiTotstring(ex.what());
		MessageBox(*this, Reason.c_str(), _T("Connection broken"), MB_OK|MB_ICONERROR);
	}
	
	return 0;
}

LRESULT ClientMainWindow::OnFdClose(WPARAM /*wParam*/, LPARAM lParam)
{
	StopGame();
	MessageBox(*this, _T("Connection closed by the server"), 
		_T("Connection broken"), MB_OK|MB_ICONERROR);
	return 0;
}

void ClientMainWindow::OnFdRead_PlayerNum()
{
	m_TcpClient.Recv(m_PlayerNum);
	m_pReadStateFun = &ClientMainWindow::OnFdRead_GameStatus;
}

void ClientMainWindow::OnFdRead_GameStatus()
{
	if (m_TcpClient.RecvPackage(m_NetworkPackage))
		OpenNetworkPackage();
}

void ClientMainWindow::OpenNetworkPackage()
{
	int NumTanks, NumProjectiles, NumExplosions;
	PlayerStatus Tank;
	ProjectileStatus Projectile;
	ExplosionStatus Explosion;
	int TeamPoints;
	BOOL ExplosionSound, FireSound;
	int PopIndex = 0;

	m_GameStatus.Clear();


	PopIndex = m_NetworkPackage.Pop(NumTanks, PopIndex);
	for (int i = 0; i < NumTanks; i++)
	{
		PopIndex = m_NetworkPackage.Pop(Tank, PopIndex);
		if ((Tank.dir >= TankInfo::NORTH) && (Tank.dir <= TankInfo::WEST)
			&& (Tank.team >= TankInfo::GREEN) && (Tank.team <= TankInfo::BLUE))
		{
			AC::tostringstream os;

			os << Tank.Nickname << _T("(") << Tank.Kills << _T("k, ")
				<< Tank.Respawns << _T("d)");

			m_GameStatus.AddTank(Tank.x, Tank.y, 
				(TankInfo::Dir)Tank.dir, (TankInfo::Team)Tank.team,
				os.str());
		}
		else
			printf("Mensaje descartado\n");
	}

	PopIndex = m_NetworkPackage.Pop(NumProjectiles, PopIndex);
	for (int i = 0; i < NumProjectiles; i++)
	{
		PopIndex = m_NetworkPackage.Pop(Projectile, PopIndex);
		m_GameStatus.AddProjectile(Projectile.x, Projectile.y);
	}

	PopIndex = m_NetworkPackage.Pop(NumExplosions, PopIndex);
	for (int i = 0; i < NumExplosions; i++)
	{
		PopIndex = m_NetworkPackage.Pop(Explosion, PopIndex);
		m_GameStatus.AddExplosion(Explosion.x, Explosion.y);
	}

	PopIndex = m_NetworkPackage.Pop(TeamPoints, PopIndex);
	m_GameStatus.SetGreenTeamPoints(TeamPoints);
	PopIndex = m_NetworkPackage.Pop(TeamPoints, PopIndex);
	m_GameStatus.SetBlueTeamPoints(TeamPoints);

	PopIndex = m_NetworkPackage.Pop(FireSound, PopIndex);
	if (FireSound)
		PlaySound((LPCTSTR)IDR_FIRE, NULL, SND_RESOURCE | SND_ASYNC);
	PopIndex = m_NetworkPackage.Pop(ExplosionSound, PopIndex);
	if (ExplosionSound)
		PlaySound((LPCTSTR)IDR_EXPLOSION, NULL, SND_RESOURCE | SND_ASYNC);

	BOOL CanShoot = (m_ReloadingTimer.Milliseconds() > SHOOT_INTERVAL);
	m_GameStatus.SetLoaded(CanShoot);

	m_NetworkPackage.Clear();
	InvalidateRect(*this, NULL, TRUE);
}

void ClientMainWindow::StopGame()
{
	if (m_TcpClient)
	{
		m_TcpClient.Close();
		m_bConnected = FALSE;
	}
}
