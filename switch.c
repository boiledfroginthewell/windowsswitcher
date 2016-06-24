#include<windows.h>
#include<stdlib.h>
#include<stdio.h>

#include "switch.h"

SWITCH_LIST initSwitchList() {
	return (SWITCH_LIST) calloc(1, sizeof(SWITCH*));
}

int newSwitch(SWITCH_LIST tgt, HWND hwnd, HWND labelHandle, char label) {
	SWITCH* next;

	if (tgt == NULL) return 1;
	
	if (*tgt == NULL) {
		next = NULL;
	} else {
		next = *tgt;
	}

	*tgt = (SWITCH*)malloc(sizeof(SWITCH));
	if (*tgt == NULL) return -1;
	(*tgt)->hwnd = hwnd;
	(*tgt)->labelHandle = labelHandle;
	(*tgt)->label = label;
	(*tgt)->next = next;
	return 0;
}

SWITCH* findSwitch(SWITCH_LIST list, char label) {
	SWITCH* iter;
	if (list == NULL || *list == NULL) return NULL;

	iter = *list;
	do {
		if (iter->label == label) return iter;
	} while ((iter = iter->next) != NULL);
	return NULL;
}


void freeSwitch(SWITCH_LIST list) {
	SWITCH* remove;
	SWITCH* next;

	if (list == NULL) return;
	remove = *list;
	while (remove != NULL) {
		next = remove->next;
		free(remove);
		remove = next;
	}
	free(list);
}
