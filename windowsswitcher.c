#include<windows.h>
#include<string.h>
#include<stdio.h>
#include<ctype.h>

#include "switch.h"

#define MAX_TITLE 512
#define LABELS "gfdsatrewqvcx12345hjkl;yuiopnm,.7890"
#define LABEL_OFFSET 3
#define SWITCH_HEIGHT 25

int nSwitches = 0;
HWND mainWin;
SWITCH_LIST switches;

void getWindowGeo(HWND hwnd, RECT *rect) {
	WINDOWPLACEMENT place;
	WINDOWPLACEMENT origin;
	RECT currentRect;

	GetWindowPlacement(hwnd, &place);
	if (((place.flags & WPF_RESTORETOMAXIMIZED) != 0) || (place.showCmd == SW_MAXIMIZE)) {
		rect->left = place.ptMaxPosition.x;
		rect->top = place.ptMaxPosition.y;
	} else {
		*rect = place.rcNormalPosition;

		/* Aero snap check */
		if (place.showCmd == SW_SHOWNORMAL) {
			GetWindowRect(hwnd, &currentRect);
			if (rect->bottom - rect->top != currentRect.bottom - currentRect.top ||
			    rect->right - rect->left != currentRect.right - currentRect.left) {
				/* Window is aero-snapped */
				*rect = currentRect;
			}
		}
	}
	/*
	GetWindowPlacement(mainWin, &place);
	rect->left -= place.rcNormalPosition.left;
	rect->top -= place.rcNormalPosition.top;
	rect->right -= place.rcNormalPosition.right;
	rect->bottom -= place.rcNormalPosition.bottom;
	*/
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	HWND labelHandler;
	TCHAR windowText[512 + LABEL_OFFSET]; // TODO: LABEL
	TCHAR class[10];
	RECT rect;
	WINDOWPLACEMENT place;
	char label;
	int i;

	if (hwnd == NULL) return TRUE;
	for (i = 0; i < sizeof(windowText); i++) {
		windowText[i] = 0;
	}
	for (i = 0; i < sizeof(class); i++) class[i] = 0;
	if (!IsWindowVisible(hwnd)) return TRUE;
	//if (GetWindow(hwnd, GW_OWNER) != NULL) return TRUE;
	GetWindowText(hwnd , windowText + LABEL_OFFSET, sizeof(windowText) - 10 - LABEL_OFFSET);
	if (windowText[LABEL_OFFSET] == TEXT('\0')) return TRUE;
	if (lstrcmp(windowText + LABEL_OFFSET, TEXT("スタート")) == 0) return TRUE;
	GetClassName(hwnd, class, sizeof(class) - 1);
	if (lstrcmp(class, TEXT("Progman")) == 0) return TRUE;
	if (nSwitches >= strlen(LABELS)) {
		/* Too many windows to handle */
		return TRUE;
	}

	/* Create switch */
	label = LABELS[nSwitches++];
	windowText[0] =toupper(label);
	windowText[1] = ':';
	windowText[2] = ' ';

	getWindowGeo(hwnd, &rect);
	GetWindowPlacement(mainWin, &place);
	rect.left -= place.rcNormalPosition.left;
	rect.top -= place.rcNormalPosition.top;
	
	// TODO: NO_ACTIVATE
	// TODO: ex_edge or border
	labelHandler = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, TEXT("Switch"), TEXT(windowText), 
		WS_POPUP | WS_BORDER, rect.left, rect.top, 400, SWITCH_HEIGHT, mainWin, NULL, (HINSTANCE)lParam, NULL);
	newSwitch(switches, hwnd, labelHandler, label);
	SetWindowLongPtr(labelHandler, GWLP_USERDATA, (LONG_PTR) *switches);
	ShowWindow(labelHandler, SW_SHOW);

	return TRUE;
}

void activate(HWND hwnd) {
	if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
	SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	//SwitchToThisWindow(hwnd, TRUE);
	//SetForegroundWindow(hwnd);
	//BringWindowToTop(sw->hwnd);
	//SetActiveWindow(sw->hwnd);
}

BOOL isFirst = TRUE;
void terminate() {
	if (isFirst) {
		isFirst = FALSE;
		freeSwitch(switches);
		switches = NULL;
		PostQuitMessage(0);
	}
}

HICON getWindowIcon(HWND hwnd) {
	ULONG_PTR icon;
	
	icon = SendMessage(hwnd, WM_GETICON, ICON_SMALL2, 0);
	if (icon != (ULONG_PTR) NULL) return (HICON) icon;
	icon = SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0);
	if (icon != (ULONG_PTR) NULL) return (HICON) icon;
	icon = SendMessage(hwnd, WM_GETICON, ICON_BIG, 0);
	if (icon != (ULONG_PTR) NULL) return (HICON) icon;
	icon = GetClassLongPtr(hwnd, GCLP_HICON);
	if (icon != (ULONG_PTR) NULL) return (HICON) icon;
	icon = GetClassLongPtr(hwnd, GCLP_HICONSM);
	if (icon !=(ULONG_PTR) NULL) return (HICON) icon;
	return NULL;
}

/* Window Procedure for the main window. */
LRESULT CALLBACK subWinProc(HWND hwnd, UINT msgCode, WPARAM wparam, LPARAM lparam) {
	SWITCH* sw;
	HDC dc;
	PAINTSTRUCT paint;
	TCHAR text[255] ={0};
	HICON icon;
	SIZE size;
	
	switch(msgCode) {
	case WM_CREATE:
		dc = GetDC(hwnd);
		GetWindowText(hwnd, text, 250);
		GetTextExtentPoint(dc, text, strlen(text), &size);
		SetWindowPos(hwnd, 0, 0, 0, 
			size.cx + SWITCH_HEIGHT + 10, SWITCH_HEIGHT, SWP_NOMOVE | SWP_NOZORDER);
		ReleaseDC(hwnd, dc);
		return 0;
	case WM_PAINT:
		dc = BeginPaint(hwnd, &paint);
		sw = (SWITCH*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		icon = getWindowIcon(sw->hwnd);
		DrawIconEx(dc, 2, 1, icon, SWITCH_HEIGHT - 4, SWITCH_HEIGHT - 4, 0, (HBRUSH)GetStockObject(WHITE_BRUSH), DI_IMAGE);
		GetWindowText(hwnd, text, 250);
		TextOut(dc, SWITCH_HEIGHT + 5, 2, text, strlen(text));
		EndPaint(hwnd, &paint);
		return 0;
	case WM_RBUTTONDOWN: // right click to close the assiated window
		sw = (SWITCH*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		ShowWindow(sw->hwnd, SW_MINIMIZE);
		terminate();
		return 0;
	case WM_MBUTTONDOWN: // wheel click to close the assiated window
		sw = (SWITCH*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		SendMessage(sw->hwnd, WM_CLOSE, 0, 0);
		terminate();
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

/* Window Procedure for the main window. */
LRESULT CALLBACK mainWinProc(HWND hwnd, UINT msgCode, WPARAM wparam, LPARAM lparam) {
	SWITCH* sw;
	TCHAR name[32] = {0};
	switch(msgCode) {
	case WM_CHAR:
		if ((sw = findSwitch(switches, ((TCHAR) wparam))) == NULL) {
			terminate();
			return 0;
		}
		activate(sw->hwnd);
		terminate();
		return 0;
	case WM_DESTROY:
		terminate();
		return 0;
	case WM_KILLFOCUS:
		GetClassName((HWND) wparam, name, 31);
		if (lstrcmp(name, TEXT("Switch")) != 0) {
			terminate();
			return 0;
		}
	default:
		return DefWindowProc(hwnd, msgCode, wparam, lparam);
	}
}

void manageSwitches(SWITCH_LIST switches) {
	SWITCH *sw;
	RECT rect;
	RECT origin;
	int limitX = 50;
	int limitY = 50;
	BOOL initialY = TRUE;
	long x, y;

	getWindowGeo(mainWin, &origin);
	limitX += origin.left;
	limitY += origin.top;
	sw = *switches;
	do {
		getWindowGeo(sw->labelHandle, &rect);
		x = rect.left;
		y = rect.top;
		if (x < origin.left) x = origin.left;
		if (y < origin.top) y = origin.top;
		if (x < limitX && y < limitY) {
			x = origin.left;
			if (initialY) {
				initialY = FALSE;
				limitY = origin.top;
			}
			y = limitY;
			limitY += rect.bottom - rect.top;
		}
		SetWindowPos(sw->labelHandle, HWND_TOPMOST, x, y, 0, 0, 
			SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	} while ((sw = sw->next) != NULL);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE dummy, PSTR lpCmdLine, int nCmdShow) {
	HWND hwnd;
	MSG msg;
	WNDCLASS class;
	WNDCLASS subWinClass;

	class.style		= 0;
	class.lpfnWndProc	= mainWinProc;
	class.cbClsExtra	= class.cbWndExtra	= 0;
	class.hInstance		= hInstance;
	class.hIcon		= LoadIcon(NULL , IDI_APPLICATION);
	class.hCursor		= LoadCursor(NULL , IDC_ARROW);
	class.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	class.lpszMenuName	= NULL;
	class.lpszClassName	= TEXT("Switcher");

	subWinClass.style		= 0;
	subWinClass.lpfnWndProc	= subWinProc;
	subWinClass.cbClsExtra = 0;
	subWinClass.cbWndExtra = sizeof(SWITCH*);
	subWinClass.hInstance		= hInstance;
	subWinClass.hIcon		= LoadIcon(NULL , IDI_APPLICATION);
	subWinClass.hCursor		= LoadCursor(NULL , IDC_ARROW);
	subWinClass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	subWinClass.lpszMenuName	= NULL;
	subWinClass.lpszClassName	= TEXT("Switch");

	if (!RegisterClass(&class)) return -1;
	if (!RegisterClass(&subWinClass)) return -1;

	mainWin = CreateWindowEx(WS_EX_TOOLWINDOW, TEXT("Switcher"), 0, 
	//mainWin = CreateWindow(TEXT("Switcher"), TEXT("rchsch\0"), 
		WS_POPUP | WS_BORDER, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if (mainWin == NULL) return -1;
	
	switches = initSwitchList();
	EnumWindows(EnumWindowsProc , (LPARAM)hInstance);
	if (nSwitches == 0) {
		terminate();
		return 0;
	} else if (nSwitches == 1) {
		activate((*switches)->hwnd);
		terminate();
		return 0;
	} else if (nSwitches == 2) {
		activate((*switches)->hwnd);
		terminate();
		return 0;
	}

	manageSwitches(switches);
	ShowWindow(mainWin, SW_SHOW);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
