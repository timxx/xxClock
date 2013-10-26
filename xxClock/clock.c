//////////////////////////////////////////////////////////////////////////
//为了使用SetLayered...
#define _WIN32_WINNT 0x0501
//////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <tchar.h>
#include <math.h>
#include <windowsx.h>
#include <Shlwapi.h>

#include "clock.h"
#include "resource.h"
//=======================================================================
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "skinxml.lib")
//////////////////////////////////////////////////////////////////////////
//=======================================================================
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	MSG msg = {0};

	if (!Register(hInstance) || !InitWnd())
		return 0;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
//=======================================================================
BOOL Register(HINSTANCE hInst)
{
	WNDCLASSEX wcex = {0};

	ghInst = hInst;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.hbrBackground	= GetStockBrush(WHITE_BRUSH);
	wcex.hInstance		= hInst;
	wcex.lpfnWndProc	= WndProc;
	wcex.lpszClassName	= szClsName;	
	wcex.lpszMenuName	= 0;
	wcex.style			= CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon			= LoadIcon(hInst, MAKEINTRESOURCE(IDI_CLOCK));

	if (!RegisterClassEx(&wcex))
		return FALSE;

	return TRUE;
}
//=======================================================================
BOOL InitWnd()
{
	SKINMENU lastSkin = {0};

	hwndMain = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
		szClsName, TEXT("Clock"),
		WS_POPUP,
		CW_USEDEFAULT, 0, 200, 200,
		NULL, NULL, ghInst, NULL);

	if (!hwndMain){
		return FALSE;
	}

	nWndWidth = nWndHeight = 200;
	centerPoint.x = 100;
	centerPoint.y = 100;

	horHand.lenght	= 60;
	horHand.width	= 3;
	horHand.color	= RGB(0, 0, 0);

	minHand.lenght	= 70;
	minHand.width	= 2;
	minHand.color	= RGB(0, 250, 0);

	secHand.lenght	= 90;
	secHand.width	= 1;
	secHand.color	= RGB(255, 0, 0);

	RestoreWndPos(hwndMain);

	//使白色部分透明化
	SetLayeredWindowAttributes(hwndMain, RGB(255, 255, 255), 0, LWA_COLORKEY);

	LoadSkins();

	if (!GetLastSkin(lastSkin.path, MAX_PATH))
	{
		nCurSkinIndex = 0;
		if (skin_menu_head){
			if (!ReadSkin(skin_menu_head->path))
				fNoImg = TRUE;
		}else{
			fNoImg = TRUE;
		}
	}
	else
	{
		nCurSkinIndex = SkinMenu_Exists(skin_menu_head, &lastSkin);

		if (!ReadSkin(lastSkin.path))
			fNoImg = TRUE;
	}

	ShowWindow(hwndMain, SW_SHOW);
	UpdateWindow(hwndMain);
	
	return TRUE;
}
//=======================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hWnd, WM_TIMER,			OnTimer);
		HANDLE_MSG(hWnd, WM_PAINT,			OnPaint);
		HANDLE_MSG(hWnd, WM_COMMAND,		OnCommand);
		HANDLE_MSG(hWnd, WM_RBUTTONUP,		OnRButtonUp);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN,	OnLButtonDown);
		HANDLE_MSG(hWnd, WM_SIZE,			OnSize);
		HANDLE_MSG(hWnd, WM_GETMINMAXINFO,	OnGetMinMaxInfo);
		HANDLE_MSG(hWnd, WM_CREATE,			OnCreate);
		HANDLE_MSG(hWnd, WM_CLOSE,			OnClose);
		HANDLE_MSG(hWnd, WM_DESTROY,		OnDestroy);
		HANDLE_MSG(hWnd, WM_ERASEBKGND,		OnEraseBkgnd);

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
//=======================================================================
void OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	if (fDigitMode)
	{
		if (!hbmpDigit){
			fDigitMode = FALSE;
			return; 
		}
		DrawDigitClock(hdc);
	}
	else
	{
		SYSTEMTIME st;
		
		GetLocalTime(&st);
		
		DrawHorHand(hdc, st.wHour + (double)st.wMinute/60);
		DrawMinHand(hdc, st.wMinute + (double)st.wSecond/60);
		DrawSecHand(hdc, st.wSecond);
	}

	EndPaint(hWnd, &ps);
}
//=======================================================================
void DrawDot(HDC hdc, int x, int y, int r)
{
	Ellipse(hdc, x - r, y - r, x + r, y + r);	
}
//=======================================================================
int CalcX( double angle, int length )
{
	return (int)(centerPoint.x + length * sin(angle));
}
//=======================================================================
int CalcY(double angle, int offset)
{
	return (int)(centerPoint.y - offset * cos(angle));
}
//=======================================================================
void DrawClockBk(HDC hdc)
{
	int x, y;
	int i;
	int offsetX, offsetY;

	HBRUSH hbr;
	HGDIOBJ hOldObj;
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 250));

	hOldObj = SelectObject(hdc, hPen);

	//最外面的大圈
	offsetX = nWndWidth/2;
	offsetY = nWndHeight/2;

	Ellipse(hdc, centerPoint.x - offsetX, centerPoint.y - offsetY,
		centerPoint.x + offsetX, centerPoint.y + offsetY);

	SelectObject(hdc, hOldObj);
	DeletePen(hPen);

	hPen = CreatePen(PS_SOLID, 20, RGB(184, 230, 29));
	hOldObj = SelectObject(hdc, hPen);
	
	offsetX -= 11;
	offsetY -= 11;
	
	Ellipse(hdc, centerPoint.x - offsetX, centerPoint.y - offsetY,
		centerPoint.x + offsetX, centerPoint.y + offsetY);

	SelectObject(hdc, hOldObj);
	DeletePen(hPen);

	hPen = CreatePen(PS_SOLID, 1, RGB(0, 34, 180));
	hOldObj = SelectObject(hdc, hPen);

	offsetX -= 5;
	offsetY -= 5;

	Ellipse(hdc, centerPoint.x - offsetX, centerPoint.y - offsetY,
		centerPoint.x + offsetX, centerPoint.y + offsetY);

	SelectObject(hdc, hOldObj);
	DeletePen(hPen);

	hbr = CreateSolidBrush(RGB(0, 255, 0));
	hOldObj = SelectObject(hdc, hbr);
	//中心点
	DrawDot(hdc, centerPoint.x, centerPoint.y, 5);

	SelectObject(hdc, hOldObj);
	DeleteObject(hbr);

	hbr = CreateSolidBrush(RGB(0, 0, 0));
	hOldObj = SelectObject(hdc, hbr);


	//时间刻度
	for (i=0; i<60; i++)
	{
		x = CalcX(i * PI/30, nWndWidth/2 - 10);
		y = CalcY(i * PI/30, nWndHeight/2 - 10);
		DrawDot(hdc, x, y, 1);
	}

	SelectObject(hdc, hOldObj);
	DeleteObject(hbr);

	hbr = CreateSolidBrush(RGB(0, 0, 255));
	hOldObj = SelectObject(hdc, hbr);

	//时刻度
	for (i=0; i<12; i++)
	{
		x = CalcX(i * PI/6, nWndWidth/2 - 12);
		y = CalcY(i * PI/6, nWndHeight/2 - 12);
		DrawDot(hdc, x, y, 2);
	}

	SelectObject(hdc, hOldObj);
	DeleteObject(hbr);

	hbr = CreateSolidBrush(RGB(255, 0, 0));
	hOldObj = SelectObject(hdc, hbr);

	//四大点
	for (i=0; i<4; i++)
	{
		x = CalcX(i * PI/2, nWndWidth/2 - 10);
		y = CalcY(i * PI/2, nWndHeight/2 - 10);
		DrawDot(hdc, x, y, 5);
	}

	SelectObject(hdc, hOldObj);
	DeleteObject(hbr);
}
//=======================================================================
void DrawHorHand(HDC hdc, double wHor)
{
	HPEN hPen = CreatePen(PS_SOLID, horHand.width, horHand.color);
	HGDIOBJ hOldObj = SelectObject(hdc, hPen);

	int x = CalcX(wHor * PI/6, horHand.lenght);
	int y = CalcY(wHor * PI/6, horHand.lenght);

	MoveToEx(hdc, centerPoint.x, centerPoint.y, NULL);
	LineTo(hdc, x, y);

	SelectObject(hdc, hOldObj);
	DeleteObject(hPen);
}
//=======================================================================
void DrawMinHand(HDC hdc, double wMin)
{
	HPEN hPen = CreatePen(PS_SOLID, minHand.width, minHand.color);
	HGDIOBJ hOldObj = SelectObject(hdc, hPen);

	int x = CalcX(wMin * PI/30, minHand.lenght);
	int y = CalcY(wMin * PI/30, minHand.lenght);

	MoveToEx(hdc, centerPoint.x, centerPoint.y, NULL);
	LineTo(hdc, x, y);

	SelectObject(hdc, hOldObj);
	DeleteObject(hPen);
}
//=======================================================================
void DrawSecHand(HDC hdc, double wSec)
{
	HPEN hPen = CreatePen(PS_SOLID, secHand.width, secHand.color);
	HGDIOBJ hOldObj = SelectObject(hdc, hPen);

	int x = CalcX(wSec * PI/30, secHand.lenght);
	int y = CalcY(wSec * PI/30, secHand.lenght);

	MoveToEx(hdc, centerPoint.x, centerPoint.y, NULL);
	LineTo(hdc, x, y);

	SelectObject(hdc, hOldObj);
	DeleteObject(hPen);
}
//=======================================================================
void ShowRMenu(HWND hWnd)
{
	POINT pt;
	SKINMENU *p = skin_menu_head;
	int count = 0;

	HMENU hMenu = LoadMenu(ghInst, MAKEINTRESOURCE(IDR_MENU_RKEY));
	HMENU hPopupMenu = GetSubMenu(hMenu, 0);
	HMENU hSubPopMenu = CreatePopupMenu();

	while (p)
	{
		if (count != nCurSkinIndex){
			AppendMenu(hSubPopMenu, MF_STRING | MF_BYCOMMAND, p->cmd, p->name);
		}else{
			AppendMenu(hSubPopMenu, MF_STRING | MF_BYCOMMAND | MF_CHECKED, p->cmd, p->name);
		}
		p = p->next;
		count++;
	}

	if (count > 0)
	{
		InsertMenu(hPopupMenu, 3, MF_POPUP | MF_STRING | MF_BYPOSITION, (UINT)hSubPopMenu, TEXT("Skin"));
		InsertMenu(hPopupMenu, 4, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
	}

	CheckMenuRadioItem(hPopupMenu, IDM_MODE_DIGIT, IDM_MODE_NRM,
		fDigitMode ? IDM_MODE_DIGIT : IDM_MODE_NRM, MF_BYCOMMAND);

	GetCursorPos(&pt);
	TrackPopupMenu(hPopupMenu, TPM_CENTERALIGN, pt.x, pt.y, 0, hWnd, NULL);

	DestroyMenu(hPopupMenu);
	DestroyMenu(hSubPopMenu);
}
//=======================================================================
void OnTimer(HWND hWnd, UINT uID)
{
	SYSTEMTIME st;

	GetLocalTime(&st);

	if (st.wSecond != wLastSec)
	{	InvalidateRect(hWnd, NULL, TRUE);

		wLastSec = st.wSecond;
		if (!fDigitMode){
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_TICK), ghInst, SND_ASYNC | SND_RESOURCE);
		}
	}
}
//=======================================================================
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	fDigitMode = GetClockMode();

	//100ms刷新，避免显示延迟过长
	SetTimer(hWnd, TIMER_ID, 100, NULL);
	
	return TRUE;
}
//=======================================================================
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{
	case IDM_MODE_DIGIT:
		if (hbmpDigit == NULL)
		{
			MessageBox(hWnd, TEXT("Can't change to digital mode!"), TEXT("xxClock"), MB_ICONERROR);
			break;
		}

		{
			BITMAP bmp;
			GetObject(hbmpDigit, sizeof(BITMAP), &bmp);
			SetWindowPos(hWnd, NULL, 0, 0, bmp.bmWidth/11*8, bmp.bmHeight, SWP_NOZORDER | SWP_NOMOVE);
		}

		fDigitMode = TRUE;

		InvalidateRect(hWnd, NULL, TRUE);
		SaveClockMode();
		break;

	case IDM_MODE_NRM:
		{
			BITMAP bmp;
			GetObject(hbmpBk, sizeof(BITMAP), &bmp);
			//必须更改回图片大小，否则显示不完整
			SetWindowPos(hWnd, NULL, 0, 0, bmp.bmWidth, bmp.bmHeight, SWP_NOZORDER | SWP_NOMOVE);

			fDigitMode = FALSE;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		SaveClockMode();
		break;

	case IDM_EXIT:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	default:
		if (id >= SKIN_MENU_BASE)
		{
			SKINMENU *skin;
			int newSkinIndex;

			if (nCurSkinIndex == id - SKIN_MENU_BASE)
				break;

			newSkinIndex = id - SKIN_MENU_BASE;

			skin = SkinMenu_GetAt(skin_menu_head, newSkinIndex);
			if (!skin)
			{
				MessageBox(hWnd, TEXT("Failed to change skin!"), TEXT("xxClock"), MB_ICONERROR);
				break;
			}

			if (!ReadSkin(skin->path)){
				MessageBox(hWnd, TEXT("Failed to load skin!"), TEXT("xxClock"), MB_ICONERROR);
			}else{
				if (fNoImg){
					fNoImg = FALSE;
				}
				nCurSkinIndex = newSkinIndex;
				SaveLastSkin();
			}
		}
		break;
	}
}
//=======================================================================
void OnClose(HWND hWnd)
{
	DestroyWindow(hWnd);
}
//=======================================================================
void OnDestroy(HWND hWnd)
{
	SaveWndPos(hWnd);
	KillTimer(hWnd, TIMER_ID);

	if (hbmpBk){
		DeleteObject(hbmpBk);
	}
	if (hbmpDigit){
		DeleteObject(hbmpDigit);
	}

	SkinMenu_Free(&skin_menu_head);

	PostQuitMessage(0);
}
//=======================================================================
void OnGetMinMaxInfo(HWND hWnd, LPMINMAXINFO lpmmi)
{
	//限制窗口最小矩形
	lpmmi->ptMinTrackSize.x = 130;
	lpmmi->ptMinTrackSize.y = 130;			
}
//=======================================================================
void OnSize(HWND hWnd, UINT state, int cx, int cy)
{
	nWndWidth = cx;
	nWndHeight = cy;
}
//=======================================================================
void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	//模拟鼠标单击客户区时也能拖动
	PostMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(x, y));
}
//=======================================================================
void OnRButtonUp(HWND hWnd, int x, int y, UINT flags)
{
	LoadSkins();
	ShowRMenu(hWnd);
}
//=======================================================================
BOOL OnEraseBkgnd(HWND hWnd, HDC hdc)
{
	HDC hdcMem = NULL;
	HGDIOBJ hOldObj = NULL;
	BITMAP bmp;
	RECT rect;
	int x, y;
	int nWidth, nHeight;

	GetClientRect(hWnd, &rect);
	FillRect(hdc, &rect, GetStockBrush(WHITE_BRUSH));

	if (fDigitMode)
	{
		return TRUE;
	}

	if (fNoImg)
	{
		DrawClockBk(hdc);
		return TRUE;
	}

	hdcMem = CreateCompatibleDC(hdc);
	hOldObj = SelectObject(hdcMem, hbmpBk);

	GetObject(hbmpBk, sizeof(BITMAP), &bmp);

	nWidth = rect.right - rect.left;
	nHeight = rect.bottom - rect.top;

	//使图片居中显示
	x = (nWidth - bmp.bmWidth)/2;
	y = (nHeight - bmp.bmHeight)/2;

	if (!BitBlt(hdc, x, y, nWidth, nHeight, hdcMem, 0, 0, SRCCOPY))
		MessageBox(hWnd, "sf", "fda", 0);

	SelectObject(hdcMem, hOldObj);

	DeleteDC(hdcMem);

	return TRUE;
}
//=======================================================================
void DrawDigitClock(HDC hdc)
{
	SYSTEMTIME st;
	HDC hdcMem;
	HGDIOBJ hOldObj;
	BITMAP bmp;
	int x, y;
	int nBmpIndex[8];
	int i;
	
	hdcMem = CreateCompatibleDC(hdc);
	GetLocalTime(&st);

	GetObject(hbmpDigit, sizeof(BITMAP), &bmp);

	//居中显示
	x = (nWndWidth - bmp.bmWidth * 8 / 11)/2;
	y = (nWndHeight - bmp.bmHeight)/2;

	nBmpIndex[0] = st.wHour/10;
	nBmpIndex[1] = st.wHour%10;
	nBmpIndex[2] = 10;
	nBmpIndex[3] = st.wMinute/10;
	nBmpIndex[4] = st.wMinute%10;
	nBmpIndex[5] = 10;
	nBmpIndex[6] = st.wSecond/10;
	nBmpIndex[7] = st.wSecond%10;

	hOldObj = SelectObject(hdcMem, hbmpDigit);

	for (i=0; i<8; i++)
	{		
		BitBlt(hdc, x, y, bmp.bmWidth/11, bmp.bmHeight, hdcMem, bmp.bmWidth/11 * nBmpIndex[i], 0, SRCCOPY);

		x += bmp.bmWidth/11;	
	}

	SelectObject(hdcMem, hOldObj);

	DeleteDC(hdcMem);
}
//=======================================================================
BOOL ExeRootDir(LPTSTR pszDir, int nMax)
{
	RtlSecureZeroMemory(pszDir, nMax);

	GetModuleFileName(ghInst, pszDir, nMax);

	return PathRemoveFileSpec(pszDir);
}
//=======================================================================
BOOL ConfigPath(LPTSTR pszPath, int nMax)
{
	RtlSecureZeroMemory(pszPath, nMax);
	if (!ExeRootDir(pszPath, nMax))
		return FALSE;

	if (pszPath[lstrlen(pszPath)-1] != TEXT('\\')){
		lstrcat(pszPath, TEXT("\\config.ini"));
	}else{
		lstrcat(pszPath, TEXT("config.ini"));
	}

	return TRUE;
}
//=======================================================================
void SaveClockMode()
{
	TCHAR szIniFile[MAX_PATH];
	ConfigPath(szIniFile, MAX_PATH);

	WritePrivateProfileString(TEXT("xxClock"),
		TEXT("Mode"),
		fDigitMode ? TEXT("1") : TEXT("0"),
		szIniFile
		);
}
//=======================================================================
BOOL GetClockMode()
{
	TCHAR szIniFile[MAX_PATH];
	if (!ConfigPath(szIniFile, MAX_PATH))
		return FALSE;

	return GetPrivateProfileInt(TEXT("xxClock"),
		TEXT("Mode"), 0, szIniFile
		);
}
//=======================================================================
void SaveWndPos(HWND hWnd)
{
	WINDOWPLACEMENT wpm = {0};
	TCHAR szIniFile[MAX_PATH];

	GetWindowPlacement(hWnd, &wpm);

	ConfigPath(szIniFile, MAX_PATH);

	WritePrivateProfileStruct(TEXT("xxClock"),
		TEXT("Window"),
		//&wpm, sizeof(WINDOWPLACEMENT),
		&wpm.rcNormalPosition, sizeof(POINT),
		szIniFile
		);
}
//=======================================================================
void RestoreWndPos(HWND hWnd)
{
	//WINDOWPLACEMENT wpm = {0};
	POINT point = {0};
	TCHAR szIniFile[MAX_PATH];

	ConfigPath(szIniFile, MAX_PATH);

	GetPrivateProfileStruct(TEXT("xxClock"),
		TEXT("Window"),
		//&wpm, sizeof(WINDOWPLACEMENT),
		&point, sizeof(POINT),
		szIniFile
		);

// 	if (wpm.showCmd == SW_HIDE){
// 		wpm.showCmd = SW_SHOW;
// 	}

	//SetWindowPlacement(hWnd, &wpm);
	SetWindowPos(hWnd, 0, point.x, point.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}
//=======================================================================
void SaveLastSkin()
{
	TCHAR szIniFile[MAX_PATH];
	SKINMENU *curSkin;

	if (nCurSkinIndex < 0)
		return ;
		
	curSkin = SkinMenu_GetAt(skin_menu_head, nCurSkinIndex);
	if (!curSkin)
		return ;

	ConfigPath(szIniFile, MAX_PATH);

	WritePrivateProfileString(TEXT("xxClock"),
		TEXT("Skin"),
		curSkin->path,
		szIniFile
		);

}
//=======================================================================
BOOL GetLastSkin(LPTSTR lpPath, DWORD dwMax)
{
	TCHAR szIniFile[MAX_PATH];
	if (!ConfigPath(szIniFile, MAX_PATH))
		return FALSE;

	if (!PathFileExists(szIniFile))
		return FALSE;

	return GetPrivateProfileString(TEXT("xxClock"),
		TEXT("Skin"),
		NULL, lpPath, dwMax,
		szIniFile) >0;
}
//=======================================================================
void LoadSkins()
{
	WIN32_FIND_DATA wfd = {0};
	HANDLE hFind;
	TCHAR szSkinDir[MAX_PATH];
	TCHAR tmp[MAX_PATH];
	int	  skinMenu_cmd = SKIN_MENU_BASE;
	SKINMENU *skinMenu;

	if (!ExeRootDir(szSkinDir, MAX_PATH))
		return ;

	lstrcat(szSkinDir, TEXT("\\Skin"));
	wsprintf(tmp, TEXT("%s\\*"), szSkinDir);

	hFind = FindFirstFile(tmp, &wfd);
	
	if (!hFind)
		return;

	skinMenu = SkinMenu_GetEnd(skin_menu_head);
	if (skinMenu)
		skinMenu_cmd = skinMenu->cmd + 1;
	
	do
	{
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			TCHAR *ext = PathFindExtension(wfd.cFileName);

			if (!ext){
				continue;
			}

			if (lstrcmpi(ext, TEXT(".skn")) == 0 ||
				lstrcmpi(ext, TEXT(".zip")) == 0
				)
			{
				TCHAR fullPath[MAX_PATH];
				SKININFO si = {0};

				wsprintf(fullPath, TEXT("%s\\%s"), szSkinDir, wfd.cFileName);

				if (!LoadSkin(fullPath))
					continue;

				if (!GetSkinInfo(&si))
				{
					FreeSkin();
					continue;
				}

				skinMenu = (SKINMENU*)calloc(1, sizeof(SKINMENU));
				skinMenu->cmd = skinMenu_cmd++;
				lstrcpy(skinMenu->name, si.name);
				lstrcpy(skinMenu->path, fullPath);
				skinMenu->next = NULL;
				
				if (SkinMenu_Exists(skin_menu_head, skinMenu) == -1){
					SkinMenu_Insert(&skin_menu_head, skinMenu);
				}

				FreeSkin();
			}
		}
		
	}while(FindNextFile(hFind, &wfd));

	FindClose(hFind);
}
//=======================================================================
HBITMAP BufferToHBITMAP(HWND hWnd, const char *buffer)
{
	LPSTR hDIB;
	LPVOID lpDIBBits;
	BITMAPFILEHEADER bfh;
	HDC hdc;
	HBITMAP hBitamp;
	
	if (!buffer){
		return NULL;
	}

	strncpy((LPSTR)&bfh,(LPSTR)buffer, sizeof(bfh));
	
	if (bfh.bfType != (*(WORD*)"BM"))
		return NULL;

	hDIB = (LPSTR)buffer + sizeof(bfh);

// 	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB;
// 	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;

	lpDIBBits = (LPSTR)buffer + ((BITMAPFILEHEADER *)buffer)->bfOffBits;

	hdc = GetDC(hWnd);
	hBitamp = CreateDIBitmap(hdc, (LPBITMAPINFOHEADER)hDIB, CBM_INIT, lpDIBBits, (LPBITMAPINFO)hDIB, DIB_RGB_COLORS);
	ReleaseDC(hWnd, hdc);

	return hBitamp;
}

BOOL ReadSkin(LPCTSTR lpSkinPath)
{
	DWORD len;
	BOOL fOk = FALSE;

	char	*bmClockBuffer = 0;
	char	*bmDigitBuffer = 0;

	BITMAP	bmp;

	if (!lpSkinPath || !PathFileExists(lpSkinPath))
		return FALSE;

	__try
	{
		if (!LoadSkin(lpSkinPath))
			__leave;
		
		if (!GetHourHand(&horHand))
			__leave;

		if (!GetMinuteHand(&minHand))
			__leave;

		if (!GetSecondHand(&secHand))
			__leave;
		
		if (!GetClockCenterPoint(&centerPoint))
			__leave;

// 		if (!GetClockRect(&rcClock))
// 			__leave;
		
		len = GetClockImage(NULL, 0);
		if (len)
		{
			bmClockBuffer = (char*)calloc(len, sizeof(char));
			GetClockImage(bmClockBuffer, len);
		}
		else
		{
			__leave;
		}	
		
		len = GetDigitImage(NULL, 0);

		if (len)
		{
			bmDigitBuffer = (char*)calloc(len, sizeof(char));
			GetDigitImage(bmDigitBuffer, len);
		}
		else{
			__leave;
		}

		SetLayeredWindowAttributes(hwndMain, GetTransparentColor(), 0, LWA_COLORKEY);

		if (hbmpBk)
		{
			DeleteObject(hbmpBk);
			hbmpBk = NULL;
		}
		if (hbmpDigit)
		{
			DeleteObject(hbmpDigit);
			hbmpDigit = NULL;
		}

		hbmpBk = BufferToHBITMAP(hwndMain, bmClockBuffer);
		hbmpDigit = BufferToHBITMAP(hwndMain, bmDigitBuffer);

		free(bmClockBuffer);
		free(bmDigitBuffer);
/*
		len = GetHourHandImage(NULL, 0);
		if (len <= 0){
			__leave;
		}
		bmDigitBuffer = (char*)calloc(len, sizeof(char));
		GetHourHandImage(bmDigitBuffer, len);

		hbmpHorHand = BufferToHBITMAP(hwndMain, bmDigitBuffer);
		free(bmDigitBuffer);

		len = GetMinuteHandImage(NULL, 0);
		if (len <= 0){
			__leave;
		}
		bmDigitBuffer = (char*)calloc(len, sizeof(char));
		GetMinuteHandImage(bmDigitBuffer, len);

		hbmpMinHand = BufferToHBITMAP(hwndMain, bmDigitBuffer);
		free(bmDigitBuffer);

		len = GetSecondHandImage(NULL, 0);
		if (len <= 0){
			__leave;
		}
		bmDigitBuffer = (char*)calloc(len, sizeof(char));
		GetSecondHandImage(bmDigitBuffer, len);

		hbmpSecHand = BufferToHBITMAP(hwndMain, bmDigitBuffer);
		free(bmDigitBuffer);
*/
		if (fDigitMode)
		{
			GetObject(hbmpDigit, sizeof(BITMAP), &bmp);
			SetWindowPos(hwndMain, 0, 0, 0, bmp.bmWidth/11*8, bmp.bmHeight, SWP_NOZORDER | SWP_NOMOVE);
		}
		else
		{
			GetObject(hbmpBk, sizeof(BITMAP), &bmp);
			SetWindowPos(hwndMain, 0, 0, 0, bmp.bmWidth, bmp.bmHeight, SWP_NOZORDER | SWP_NOMOVE);
		}

		InvalidateRect(hwndMain, NULL, TRUE);

		fOk = TRUE;
	}
	__finally
	{
		FreeSkin();
	}

	return fOk;
}
/*
// GetRotatedBitmapNT	- Create a new bitmap with rotated image
// Returns		- Returns new bitmap with rotated image
// hBitmap		- Bitmap to rotate
// radians		- Angle of rotation in radians
// clrBack		- Color of pixels in the resulting bitmap that do
//			  not get covered by source pixels
HBITMAP RotatedBitmap(HBITMAP hBitmap, float radians, COLORREF clrBack)
{
	// Create a memory DC compatible with the display
	HDC sourceDC, destDC;
	BITMAP bm;

	float cosine, sine;

	int x1, y1, x2, y2, x3, y3;

	int minx, miny, maxx, maxy;

	int w, h;

	HBITMAP hbmResult, hbmOldSource, hbmOldDest;

	HBRUSH hbrBack, hbrOld;
	XFORM xform;

	sourceDC = CreateCompatibleDC( NULL );
	destDC = CreateCompatibleDC( NULL );
	
	// Get logical coordinates
	GetObject( hBitmap, sizeof( bm ), &bm );
	
	cosine = (float)cos(radians);
	sine = (float)sin(radians);
	
	// Compute dimensions of the resulting bitmap
	// First get the coordinates of the 3 corners other than origin
	x1 = (int)(bm.bmHeight * sine);
	y1 = (int)(bm.bmHeight * cosine);
	x2 = (int)(bm.bmWidth * cosine + bm.bmHeight * sine);
	y2 = (int)(bm.bmHeight * cosine - bm.bmWidth * sine);
	x3 = (int)(bm.bmWidth * cosine);
	y3 = (int)(-bm.bmWidth * sine);
	
	minx = min(0,min(x1, min(x2,x3)));
	miny = min(0,min(y1, min(y2,y3)));
	maxx = max(0,max(x1, max(x2,x3)));
	maxy = max(0,max(y1, max(y2,y3)));
	
	w = maxx - minx;
	h = maxy - miny;
	
	// Create a bitmap to hold the result
	hbmResult = CreateCompatibleBitmap(GetDC(NULL), w, h);
	
	hbmOldSource = (HBITMAP)SelectObject( sourceDC, hBitmap );
	hbmOldDest = (HBITMAP)SelectObject( destDC, hbmResult );
	
	// Draw the background color before we change mapping mode
	hbrBack = CreateSolidBrush( clrBack );
	hbrOld = (HBRUSH)SelectObject( destDC, hbrBack );
	PatBlt(destDC, 0, 0, w, h, PATCOPY );
	DeleteObject( SelectObject( destDC, hbrOld ) );
	
	// We will use world transform to rotate the bitmap
	SetGraphicsMode(destDC, GM_ADVANCED);

	xform.eM11 = cosine;
	xform.eM12 = -sine;
	xform.eM21 = sine;
	xform.eM22 = cosine;
	xform.eDx = (float)-minx;
	xform.eDy = (float)-miny;
	
	SetWorldTransform( destDC, &xform );
	
	// Now do the actual rotating - a pixel at a time
	BitBlt(destDC, 0, 0, bm.bmWidth, bm.bmHeight, sourceDC, 0, 0, SRCCOPY );
	
	// Restore DCs
	SelectObject( sourceDC, hbmOldSource );
	SelectObject( destDC, hbmOldDest );
	
	return hbmResult;
}
*/