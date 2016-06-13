#include<windows.h>
#include<stdlib.h>

#include "switch.h"

int newSwitch(SWITCH_LIST tgt, HWND hwnd, char label) {
	SWITCH* next;

	if (tgt == NULL || *tgt == NULL) return 1;

	next = (*tgt)->next;
	*tgt = (SWITCH*)malloc(sizeof(SWITCH));
	if (tgt == NULL) return -1;
	(*tgt)->hwnd = hwnd;
	(*tgt)->label = label;
	(*tgt)->next = next;
	return 0;
}

void freeSwitch(SWITCH_LIST list) {
	SWITCH* remove;
	SWITCH* next;

	if (list == NULL || *list == NULL) return;
	remove = *list;
	while (remove != NULL) {
		next = remove->next;
		free(remove);
	}
}
