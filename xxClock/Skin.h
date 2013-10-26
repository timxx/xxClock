
#pragma once

#ifndef __SKIN_H__
#define __SKIN_H__

#include <Windows.h>

typedef struct _skin_menu
{
	TCHAR	name[128];
	int		cmd;
	TCHAR	path[MAX_PATH];

	struct _skin_menu *next;

}SKINMENU;

void		SkinMenu_Insert(SKINMENU **head, SKINMENU *newMenu);
void		SkinMenu_Free(SKINMENU **head);
SKINMENU *	SkinMenu_GetAt(SKINMENU *head, int i);
SKINMENU *	SkinMenu_GetEnd(SKINMENU *head);
int			SkinMenu_Exists(SKINMENU *head, const SKINMENU *sm);

#endif