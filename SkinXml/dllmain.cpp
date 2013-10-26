
#define SKINAPI extern "C" __declspec(dllexport)

#include "skinxml.h"
#include "tinyxml/tinyxml.h"
#include "zip/unzip.h"

TiXmlDocument *pSkinDoc = 0;
HZIP hZip = NULL;

COLORREF strtocolor(const char *str);
DWORD	 UnzipItem(const char *itemname, char *buffer, DWORD buflen);

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD dwReson, LPVOID lpReserved)
{
	switch(dwReson)
	{
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		FreeSkin();
		break;
	}
	return TRUE;
}

int atow(const char *astr, wchar_t *wstr, int nDstLen)
{
	if (wstr == NULL)
		return MultiByteToWideChar( CP_ACP, 0, astr, -1, NULL, 0 );
	
	RtlZeroMemory(wstr, nDstLen);
	
	return MultiByteToWideChar( CP_ACP, 0, astr, -1, wstr, nDstLen );
}

int wtoa(const wchar_t *wstr, char *astr, int nDstLen)
{
	if (astr == NULL)
		return WideCharToMultiByte( CP_ACP, 0, wstr, -1, astr, 0, NULL, NULL );
	
	RtlZeroMemory(astr, nDstLen);
	
	return WideCharToMultiByte(CP_ACP, 0, wstr, -1, astr, nDstLen, NULL, NULL);
}

BOOL GetPointer(const char *name, PCLOCKPOINTER pPointer);

char *GetXmlBuffer(LPCTSTR pszSkinPath)
{
	hZip = OpenZip(pszSkinPath, NULL);

	if (!hZip){
		return NULL;
	}
	
	int index;
	ZIPENTRY ze;
	
	FindZipItem(hZip, TEXT("Skin.xml"), true, &index, &ze);
	
	if (index == -1)
		return NULL;

	char *buf = new char[ze.unc_size+1];
	if (UnzipItem(hZip, index, buf, ze.unc_size) != ZR_OK)
	{
		delete [] buf;

		return NULL;
	}

	return buf;
}

BOOL LoadSkin(LPCTSTR pszSkinPath)
{
	FreeSkin();

	pSkinDoc = new TiXmlDocument;

	char *buffer = GetXmlBuffer(pszSkinPath);

	if (!buffer){
		return FALSE;
	}

	pSkinDoc->Parse(buffer);

	delete [] buffer;

	return !pSkinDoc->Error();
}

void FreeSkin()
{
	if (pSkinDoc)
	{
		delete [] pSkinDoc;
		pSkinDoc = 0;
	}

	if (hZip)
	{
		CloseZip(hZip);
		hZip = NULL;
	}
}

/*
* <Skin version = "" name = "" author = "" info = "" />
*/
BOOL GetSkinInfo(PSKININFO pSkinInfo)
{
	if (!pSkinDoc){		
		return FALSE;
	}

	TiXmlElement *skinElement = pSkinDoc->FirstChildElement("Skin");

	if (!skinElement){
		return FALSE;
	}

	const char *ver		= skinElement->Attribute("version");
	const char *name	= skinElement->Attribute("name");
	const char *author	= skinElement->Attribute("author");
	const char *info	= skinElement->Attribute("info");

	ZeroMemory(pSkinInfo, sizeof(SKININFO));

#ifdef UNICODE
	if (ver)
	{
		int len = atow(ver, NULL, 0);
		if (len)
		{
			wchar_t *buffer = new wchar_t[len+1];
			atow(ver, buffer, len+1);
			lstrcpyW(pSkinInfo->ver, buffer);
			delete [] buffer;
		}
	}

	if (name)
	{
		int len = atow(name, NULL, 0);
		if (len)
		{
			wchar_t *buffer = new wchar_t[len+1];
			atow(name, buffer, len+1);
			lstrcpyW(pSkinInfo->name, buffer);
			delete [] buffer;
		}
	}

	if (author)
	{
		int len = atow(author, NULL, 0);
		if (len)
		{
			wchar_t *buffer = new wchar_t[len+1];
			atow(author, buffer, len+1);
			lstrcpyW(pSkinInfo->author, buffer);
			delete [] buffer;
		}
	}

	if (info)
	{
		int len = atow(ver, NULL, 0);
		if (len)
		{
			wchar_t *buffer = new wchar_t[len+1];
			atow(info, buffer, len+1);
			lstrcpyW(pSkinInfo->info, buffer);
			delete [] buffer;
		}
	}

#else

	if (ver){
		lstrcpyA(pSkinInfo->ver, ver);
	}

	if (name){
		lstrcpyA(pSkinInfo->name, name);
	}

	if (author){
		lstrcpyA(pSkinInfo->author, author);
	}

	if (info){
		lstrcpyA(pSkinInfo->info, info);
	}

#endif

	return TRUE;
}

COLORREF GetTransparentColor()
{
	if (!pSkinDoc){		
		return RGB(255, 255, 255);
	}
	
	TiXmlElement *skinElement = pSkinDoc->FirstChildElement("Skin");
	
	if (!skinElement){
		return RGB(255, 255, 255);
	}
	
	const char *color = skinElement->Attribute("transparent_color");

	if (!color){
		return RGB(255, 255, 255);
	}

	return strtocolor(color);
}

DWORD GetClockImage(char *buffer, DWORD buflen)
{
	if (!pSkinDoc || !hZip){
		return 0;
	}

	TiXmlElement *clock = pSkinDoc->FirstChildElement("Skin");
	if (!clock){
		return 0;
	}

	clock = clock->FirstChildElement("clock");
	if (!clock){
		return 0;
	}

	const char *image = clock->Attribute("image");

	return UnzipItem(image, buffer, buflen);
}

BOOL GetClockCenterPoint(LPPOINT pPoint)
{
	if (!pSkinDoc){
		return 0;
	}
	
	TiXmlElement *clock = pSkinDoc->FirstChildElement("Skin");
	if (!clock){
		return FALSE;
	}
	
	clock = clock->FirstChildElement("clock");
	if (!clock){
		return FALSE;
	}
	
	TiXmlElement *centerElement = clock->FirstChildElement("center");
	if (!centerElement){
		return FALSE;
	}
	
	centerElement->Attribute("x",	 (int*)&pPoint->x);
	centerElement->Attribute("y",	 (int*)&pPoint->y);

	return TRUE;
}

BOOL GetHourHand(PCLOCKPOINTER pHor)
{
	return GetPointer("hour", pHor);
}

BOOL GetMinuteHand(PCLOCKPOINTER pMin)
{
	return GetPointer("minute", pMin);
}

BOOL GetSecondHand(PCLOCKPOINTER pSec)
{
	return GetPointer("second", pSec);
}

DWORD GetSound(char *buffer, DWORD buflen)
{
	if (!pSkinDoc || !hZip){
		return 0;
	}
	
	TiXmlElement *clock = pSkinDoc->FirstChildElement("Skin");
	if (!clock){
		return 0;
	}
	
	clock = clock->FirstChildElement("clock");
	if (!clock){
		return 0;
	}
	
	TiXmlElement *soundElement = clock->FirstChildElement("sound");
	if (!soundElement){
		return 0;
	}

	const char *name = soundElement->GetText();

	return UnzipItem(name, buffer, buflen);
}

DWORD GetDigitImage(char *buffer, DWORD buflen)
{
	if (!pSkinDoc || !hZip){
		return 0;
	}
	
	TiXmlElement *digit = pSkinDoc->FirstChildElement("Skin");
	if (!digit){
		return 0;
	}
	
	digit = digit->FirstChildElement("digit");
	if (!digit){
		return 0;
	}
		
	const char *name = digit->Attribute("image");

	return UnzipItem(name, buffer, buflen);
}

int hexchartodec(char c)
{
	if (c>='0' && c<='9')
		return c - '0';
	
	if (c>='a' && c<='f')
		return c - 'a' + 10;
	
	if (c>='A' && c<='F')
		return c - 'A' + 10;
	
	return 0;
}

COLORREF strtocolor(const char *str)
{
	BYTE r = 0,
		 g = 0,
		 b = 0;
	
	const char *p = str;
	
	if (strlen(str) != 7 || *p != '#')
		return RGB(0, 0, 0);
	
	p++;
	
	char value[2] = {0};
	value[0] = *p++;
	value[1] = *p++;
	
	r = hexchartodec(value[0])*16 + hexchartodec(value[1]);
	
	value[0] = *p++;
	value[1] = *p++;
	
	g = hexchartodec(value[0])*16 + hexchartodec(value[1]);
	
	value[0] = *p++;
	value[1] = *p;
	
	b = hexchartodec(value[0])*16 + hexchartodec(value[1]);
	
	return RGB(r, g, b);
}

BOOL GetPointer(const char *name, PCLOCKPOINTER pPointer)
{
	if (!pSkinDoc){
		return 0;
	}
	
	TiXmlElement *clock = pSkinDoc->FirstChildElement("Skin");
	if (!clock){
		return FALSE;
	}
	
	clock = clock->FirstChildElement("clock");
	if (!clock){
		return FALSE;
	}
	
	TiXmlElement *pointerElement = clock->FirstChildElement(name);
	if (!pointerElement){
		return FALSE;
	}
	
	pointerElement->Attribute("length",	 (int*)&pPointer->lenght);
	pointerElement->Attribute("width",	 (int*)&pPointer->width);
	const char *color = pointerElement->Attribute("color");
	
	if (color)	{
		pPointer->color = strtocolor(color);
	}
	
	return TRUE;
}

DWORD UnzipItem(const char *itemname, char *buffer, DWORD buflen)
{
	int index;
	ZIPENTRY ze;

	if (!itemname){
		return 0;
	}
	
#ifdef UNICODE
	int len = atow(itemname, NULL, 0);
	if (len <= 0)
		return 0;
	wchar_t *tmp = new wchar_t[len+1];
	atow(itemname, tmp, len+1);
	FindZipItem(hZip, tmp, true, &index, &ze);
	
	delete [] tmp;

#else
	FindZipItem(hZip, itemname, true, &index, &ze);

#endif
	
	if (index == -1)
		return 0;
	
	if (!buffer){
		return ze.unc_size + 1;
	}
	
	if (UnzipItem(hZip, index, buffer, ze.unc_size) != ZR_OK)
		return 0;

	return ze.unc_size;
}
/*
DWORD GetHandImage(const char *name, char *buffer, DWORD buflen)
{
	if (!name){
		return 0;
	}

	if (!pSkinDoc || !hZip){
		return 0;
	}
	
	TiXmlElement *item = pSkinDoc->FirstChildElement("Skin");
	if (!item){
		return 0;
	}
	
	item = item->FirstChildElement("clock");
	if (!item){
		return 0;
	}
	
	item = item->FirstChildElement(name);
	if (!item){
		return 0;
	}

	const char *image = item->Attribute("image");
	
	return UnzipItem(image, buffer, buflen);
}

DWORD GetHourHandImage(char *buffer, DWORD buflen)
{
	return GetHandImage("hour_hand", buffer, buflen);
}

DWORD GetMinuteHandImage(char *buffer, DWORD buflen)
{
	return GetHandImage("minute_hand", buffer, buflen);
}

DWORD GetSecondHandImage(char *buffer, DWORD buflen)
{
	return GetHandImage("second_hand", buffer, buflen);
}*/