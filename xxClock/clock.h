//////////////////////////////////////////////////////////////////////////
#include "Skin.h"
#include "../SkinXml/SkinXml.h"
//////////////////////////////////////////////////////////////////////////
#define PI				3.14159265
#define TIMER_ID		1
#define SKIN_MENU_BASE	45000

//////////////////////////////////////////////////////////////////////////
const TCHAR szClsName[] = TEXT("xxClock");	//����
HINSTANCE ghInst = NULL;
HWND hwndMain = NULL;

int nWndWidth	= 0,	//���ڿ��
	nWndHeight  = 0;

WORD	wLastSec = 0;

//����ͼƬ
HBITMAP hbmpBk = NULL;
//����ͼƬ+':'
//Jan. 18, 2011�޸�Ϊ��ͼƬ
//HBITMAP hbmpDigit[11] = {NULL};
HBITMAP hbmpDigit = NULL;

HBITMAP hbmpHorHand = NULL;
HBITMAP hbmpMinHand = NULL;
HBITMAP hbmpSecHand = NULL;

BOOL fNoImg		= FALSE;	//����Ƿ����Ƥ��
BOOL fDigitMode	= FALSE;	//�Ƿ��Ե������ַ�ʽ��ʾ

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
//��ʾ�Ҽ������˵�
void ShowRMenu(HWND hWnd);

//��(x, y)�����뾶Ϊr�ĵ�
void DrawDot(HDC hdc, int x, int y, int r);
//�����Բ��(nCenterX, nCenterY)�ڽǶ�angle��ˮƽX, ͬʱ��ȥoffset
int CalcX(double angle, int length);
int  CalcY(double angle, int offset);

//ʱ�ӿ̶�
void DrawClockBk(HDC hdc);

//��ʱ\��\��
void DrawHorHand(HDC hdc, double wHor);
void DrawMinHand(HDC hdc, double wMin);
void DrawSecHand(HDC hdc, double wSec);

void DrawDigitClock(HDC hdc);
//////////////////////////////////////////////////////////////////////////
BOOL ExeRootDir(LPTSTR pszDir, int nMax);		//ȡ�ó����Ŀ¼
BOOL ConfigPath(LPTSTR pszPath, int nMax);		//config.ini�ļ�·��

void SaveClockMode();	//������ʾģʽ
BOOL GetClockMode();	//������ʾģʽ, TRUE--���ַ�ʽ

void SaveWndPos(HWND hWnd);		//��¼����λ��
void RestoreWndPos(HWND hWnd);	//��ԭ����λ��

void SaveLastSkin();
BOOL GetLastSkin(LPTSTR lpPath, DWORD dwMax);

void LoadSkins();
BOOL ReadSkin(LPCTSTR lpSkinPath);

HBITMAP BufferToHBITMAP(HWND hWnd, const char *buffer);
//HBITMAP RotatedBitmap(HBITMAP hBitmap, float radians, COLORREF clrBack);
//////////////////////////////////////////////////////////////////////////
