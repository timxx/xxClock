//////////////////////////////////////////////////////////////////////////
#include "Skin.h"
#include "../SkinXml/SkinXml.h"
//////////////////////////////////////////////////////////////////////////
#define PI				3.14159265
#define TIMER_ID		1
#define SKIN_MENU_BASE	45000

//////////////////////////////////////////////////////////////////////////
const TCHAR szClsName[] = TEXT("xxClock");	//类名
HINSTANCE ghInst = NULL;
HWND hwndMain = NULL;

int nWndWidth	= 0,	//窗口宽高
	nWndHeight  = 0;

WORD	wLastSec = 0;

//背景图片
HBITMAP hbmpBk = NULL;
//数字图片+':'
//Jan. 18, 2011修改为单图片
//HBITMAP hbmpDigit[11] = {NULL};
HBITMAP hbmpDigit = NULL;

HBITMAP hbmpHorHand = NULL;
HBITMAP hbmpMinHand = NULL;
HBITMAP hbmpSecHand = NULL;

BOOL fNoImg		= FALSE;	//标记是否存在皮肤
BOOL fDigitMode	= FALSE;	//是否以电子数字方式显示

CLOCKPOINTER horHand = {0};
CLOCKPOINTER minHand = {0};
CLOCKPOINTER secHand = {0};

POINT	centerPoint = {0};

SKINMENU *skin_menu_head = NULL;
int		nCurSkinIndex = -1;
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Register(HINSTANCE hInst);
BOOL InitWnd();

void OnPaint(HWND hWnd);
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);
void OnClose(HWND hWnd);
void OnDestroy(HWND hWnd);
void OnTimer(HWND hWnd, UINT uID);
void OnGetMinMaxInfo(HWND hWnd, LPMINMAXINFO lpmmi);
void OnSize(HWND hWnd, UINT state, int cx, int cy);
void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnRButtonUp(HWND hWnd, int x, int y, UINT flags);
BOOL OnEraseBkgnd(HWND hWnd, HDC hdc);
//////////////////////////////////////////////////////////////////////////
//显示右键弹出菜单
void ShowRMenu(HWND hWnd);

//在(x, y)处画半径为r的点
void DrawDot(HDC hdc, int x, int y, int r);
//计算距圆心(nCenterX, nCenterY)在角度angle的水平X, 同时减去offset
int CalcX(double angle, int length);
int  CalcY(double angle, int offset);

//时钟刻度
void DrawClockBk(HDC hdc);

//画时\分\秒
void DrawHorHand(HDC hdc, double wHor);
void DrawMinHand(HDC hdc, double wMin);
void DrawSecHand(HDC hdc, double wSec);

void DrawDigitClock(HDC hdc);
//////////////////////////////////////////////////////////////////////////
BOOL ExeRootDir(LPTSTR pszDir, int nMax);		//取得程序根目录
BOOL ConfigPath(LPTSTR pszPath, int nMax);		//config.ini文件路径

void SaveClockMode();	//保存显示模式
BOOL GetClockMode();	//返回显示模式, TRUE--数字方式

void SaveWndPos(HWND hWnd);		//记录窗口位置
void RestoreWndPos(HWND hWnd);	//还原窗口位置

void SaveLastSkin();
BOOL GetLastSkin(LPTSTR lpPath, DWORD dwMax);

void LoadSkins();
BOOL ReadSkin(LPCTSTR lpSkinPath);

HBITMAP BufferToHBITMAP(HWND hWnd, const char *buffer);
//HBITMAP RotatedBitmap(HBITMAP hBitmap, float radians, COLORREF clrBack);
//////////////////////////////////////////////////////////////////////////
