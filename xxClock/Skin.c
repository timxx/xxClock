
#include "Skin.h"

void SkinMenu_Insert(SKINMENU **head, SKINMENU *newMenu)
{
	SKINMENU *p = NULL, *q = NULL;

	if (!*head)
	{
		newMenu->next = NULL;
		*head = newMenu;
		(*head)->next = NULL;

		return ;
	}
	
	p = (*head);

	while(p)
	{
		q = p;
		p = p->next;
	}
	
	if (q)
	{
		newMenu->next = NULL;
		q->next = newMenu;
	}

}
void SkinMenu_Free(SKINMENU **head)
{
	SKINMENU *prev = 0;
	
	while(*head)
	{
		prev = *head;
		*head = (*head)->next;
		
		free(prev);
	}
	
	*head = 0;
}
SKINMENU * SkinMenu_GetAt(SKINMENU *head, int i)
{
	int j = 0;
	SKINMENU *p = 0;
	
	if (!head || i<0){
		return NULL;
	}
	
	p = head;
	
	while (p && j < i)
	{
		p = p->next;
		j++;
	}
	
	if (p && j > i)
		return NULL;
	
	return p;
}

SKINMENU *SkinMenu_GetEnd(SKINMENU *head)
{
	SKINMENU *p, *q = NULL;

	p = head;

	while (p)
	{
		q = p;
		p = p->next;
	}

	return q;
}

int SkinMenu_Exists(SKINMENU *head, const SKINMENU *sm)
{
	int index = 0;
	SKINMENU *p = head;

	if (!head || !sm){
		return -1;
	}

	while (p)
	{
		if (sm->cmd == p->cmd ||
			lstrcmpi(sm->path, p->path) == 0
			)
			return index;

		index++;
		p = p->next;
	}

	return -1;
}