#include<windows.h>
#include<stdlib.h>

#include "switch.h"

int newSwitch(SWITCH_LIST tgt, HWND hwnd, char label) {
	SWITCH* next;

	if (tgt == NULL) return 1;
	
	if (*tgt == NULL) {
		tgt = (SWITCH*)malloc(sizeof(SWITCH));

	next = (*tgt)->next;
	*tgt = (SWITCH*)malloc(sizeof(SWITCH));
	if (tgt == NULL) return -1;
	(*tgt)->hwnd = hwnd;
	(*tgt)->label = label;
	(*tgt)->next = next;
	return 0;
}

SWITCH* findSwitch(SWITCH_LIST list, char label) {
	if (list == NULL || *list == NULL) return NULL;
	do {
		if ((*list)->label == label) return *list;
	} while ((*list)->next != NULL);
	return NULL;
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
