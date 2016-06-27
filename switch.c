#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<windows.h>


#include "switch.h"
#include "windowsswitcher.h"
#include "winapiutil.h"

static void paintWindow(HWND hwnd); 

static int nSwitches = 0;


BOOL makeSubWinClass(HINSTANCE hInstance) {
	WNDCLASS subWinClass;

	subWinClass.style		= 0;
	subWinClass.lpfnWndProc	= subWinProc;
	subWinClass.cbClsExtra = 0;
	subWinClass.cbWndExtra = sizeof(SWITCH*);
	subWinClass.hInstance		= hInstance;
	subWinClass.hIcon		= LoadIcon(NULL , IDI_APPLICATION);
	subWinClass.hCursor		= LoadCursor(NULL , IDC_ARROW);
	subWinClass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	subWinClass.lpszMenuName	= NULL;
	subWinClass.lpszClassName	= SUB_WIN_CLASS_NAME;

	return RegisterClass(&subWinClass);
}


/* Window Procedure for the switch windows. */
LRESULT CALLBACK subWinProc(HWND hwnd, UINT msgCode, WPARAM wparam, LPARAM lparam) {
	SWITCH* sw;
	
	switch(msgCode) {
	case WM_PAINT:
		paintWindow(hwnd);
		return 0;
	case WM_RBUTTONDOWN: // right click to close the assiated window
		sw = (SWITCH*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		ShowWindow(sw->hwnd, SW_MINIMIZE);
		//activate(GetParent(hwnd));
		terminate();
		return 0;
	case WM_MBUTTONDOWN: // wheel click to close the assiated window
		sw = (SWITCH*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		SendMessage(sw->hwnd, WM_CLOSE, 0, 0);
		//terminate();
		sw->label = '\0';
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		activate(GetParent(hwnd));
		return 0;
	case WM_LBUTTONDOWN: // left click to close the assiated window
		sw = (SWITCH*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		activate(sw->hwnd);
		terminate();
		return 0;
	default:
		return DefWindowProc(hwnd, msgCode, wparam, lparam);
	}
}

static void paintWindow(HWND hwnd) {
	HDC dc;
	PAINTSTRUCT paint;
	HICON icon;
	TCHAR text[MAX_SWITCH_TEXT];
	SIZE size;
	SWITCH* sw;

	dc = BeginPaint(hwnd, &paint);
	/* Write the text */
	GetWindowText(hwnd, text, MAX_SWITCH_TEXT);
	GetTextExtentPoint(dc, text, strlen(text), &size);
	TextOut(dc, PADDING_L + size.cy + ICON_SPACING, PADDING_T, text, strlen(text));
	/* Draw the icon */
	sw = (SWITCH*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
	icon = (HICON) getWindowIcon(sw->hwnd);
	DrawIconEx(dc, PADDING_L, PADDING_T, icon, 
			size.cy, size.cy, 
			0, (HBRUSH)GetStockObject(WHITE_BRUSH), DI_NORMAL);

	EndPaint(hwnd, &paint);
}


/* Arranges the position of switch windows */
void manageSwitches(SWITCH_LIST switches) {
	SWITCH *sw;
	RECT location;
	RECT switchSize;
	RECT origin;
	int snapX = SNAP_X;
	int snapY = SNAP_Y;
	BOOL initialY = TRUE;
	long x, y;


	// get left top coordinates without the taskbar
	getWindowGeo(NULL, &origin);
	sw = *switches;
	do {
		// location of associated window
		getWindowGeo(sw->hwnd, &location);
		x = location.left;
		y = location.top;
		// snap to left top if the window is near there
		if ((x < origin.left + snapX) && (y < snapY + snapY)) {
			x = origin.left;
			if (initialY) {
				initialY = FALSE;
				snapY = origin.top;
			}
			y = snapY;
			GetWindowRect(sw->labelHandle, &switchSize);
			snapY += switchSize.bottom - switchSize.top - 1;
		}
		SetWindowPos(sw->labelHandle, HWND_TOPMOST, x, y, 0, 0, 
			SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
		ShowWindow(sw->labelHandle, SW_SHOW);
	} while ((sw = sw->next) != NULL);
}



SWITCH_LIST initSwitchList() {
	return (SWITCH_LIST) calloc(1, sizeof(SWITCH*));
}

int newSwitch(SWITCH_LIST tgt, HWND hwnd, HWND parent, HINSTANCE hInstance) {
	SWITCH* next;
	HWND labelHandle;
	RECT rect;
	WINDOWPLACEMENT place;
	TCHAR text[MAX_SWITCH_TEXT];
	HDC dc;
	SIZE size;

	if (tgt == NULL) return 1;
	
	if (nSwitches >= strlen(LABELS)) {
		/* Too many windows to handle */
		return 1;
	}

	/* Create a switch window */
	labelHandle = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, SUB_WIN_CLASS_NAME, text,
		WS_POPUP | WS_BORDER, 0, 0, 10, 10, parent, NULL, hInstance, NULL);

	/* Instantiate a SWITCH */
	next = *tgt;
	*tgt = (SWITCH*)malloc(sizeof(SWITCH));
	if (*tgt == NULL) return -1;
	(*tgt)->hwnd = hwnd;
	(*tgt)->label = LABELS[nSwitches++];
	(*tgt)->labelHandle = labelHandle;
	(*tgt)->next = next;

	SetWindowLongPtr(labelHandle, GWLP_USERDATA, (LONG_PTR) *tgt);

	/* Set the text to be shown in the switch */
	text[0] = toupper((*tgt)->label);
	text[1] = ':';
	text[2] = ' ';
	GetWindowText(hwnd, text + LABEL_OFFSET, MAX_SWITCH_TEXT - LABEL_OFFSET);
	SetWindowText(labelHandle, text);

	/* Determine the size of the switch window */
	dc = GetDC(labelHandle);
	GetTextExtentPoint(dc, text, strlen(text), &size);
	SetWindowPos(labelHandle, 0, 0, 0, 
		PADDING_L + size.cy + ICON_SPACING + size.cx + PADDING_R,
		PADDING_T + size.cy + PADDING_B,
		SWP_NOMOVE | SWP_NOZORDER);
	ReleaseDC(labelHandle, dc);

	return 0;
}

int countSwitches() {
	return nSwitches;
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
