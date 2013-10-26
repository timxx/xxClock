
#pragma once

#ifndef __SKIN_XML_H__
#define __SKIN_XML_H__

#ifndef SKINAPI

 	#ifdef _cplusplus
		#define SKINAPI extern "C" __declspec(dllimport)
 	#else
 		#define SKINAPI __declspec(dllimport)
 	#endif

#endif	//SKINAPI

#include <Windows.h>

typedef struct _SKIN
{
	TCHAR ver[10];
	TCHAR name[32];
	TCHAR author[32];
	TCHAR info[256];

}SKININFO, *PSKININFO;

typedef struct _POINTER
{
	int lenght;
	int width;
	COLORREF color;
}CLOCKPOINTER, *PCLOCKPOINTER;

SKINAPI BOOL	 LoadSkin(LPCTSTR pszSkinPath);
SKINAPI void	 FreeSkin();

SKINAPI BOOL	 GetSkinInfo(PSKININFO pSkinInfo);
SKINAPI COLORREF GetTransparentColor();
SKINAPI DWORD	 GetClockImage(char *buffer, DWORD buflen);
SKINAPI BOOL	 GetClockCenterPoint(LPPOINT pPoint);
SKINAPI BOOL	 GetHourHand(PCLOCKPOINTER pHor);
SKINAPI BOOL	 GetMinuteHand(PCLOCKPOINTER pMin);
SKINAPI BOOL	 GetSecondHand(PCLOCKPOINTER pSec);
SKINAPI DWORD	 GetSound(char *buffer, DWORD buflen);

// SKINAPI DWORD	 GetHourHandImage(char *buffer, DWORD buflen);
// SKINAPI DWORD	 GetMinuteHandImage(char *buffer, DWORD buflen);
// SKINAPI DWORD	 GetSecondHandImage(char *buffer, DWORD buflen);

SKINAPI DWORD	 GetDigitImage(char *buffer, DWORD buflen);

#endif	//__SKIN_XML_H__



















