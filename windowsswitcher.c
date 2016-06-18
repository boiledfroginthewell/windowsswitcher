#include<windows.h>
#include<stdio.h>
#include<ctype.h>

#include "switch.h"

#define MAX_TITLE 512
#define LABELS "gfdsatrewqvcx12345hjkl;yuiopnm,.7890"
#define LABEL_OFFSET 3

int nSwitches = 0;
HWND mainWin;
SWITCH_LIST switches;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	HWND labelHandler;
	TCHAR windowText[512 + LABEL_OFFSET]; // TODO: LABEL
	RECT rect;
	TCHAR class[10];
	WINDOWPLACEMENT place;
	MONITORINFO monitorinfo;
	char label;
	POINT point;

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

	POINT origin;
	origin.x = 0;
	origin.y = 0;
	GetWindowPlacement(hwnd, &place);
	switch (place.showCmd) {
	case SW_MAXIMIZE:
		point.x = place.ptMaxPosition.x;
		point.y = place.ptMaxPosition.y;
		break;
	default:
		point.x = place.rcNormalPosition.left;
		point.y = place.rcNormalPosition.top;
	}
	GetWindowPlacement(mainWin, &place);
	point.x -= place.rcNormalPosition.left;
	point.y -= place.rcNormalPosition.top;
	
	// TODO: ex_edge or border
	labelHandler = CreateWindowEx(WS_EX_TOPMOST | WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW, TEXT("STATIC"), TEXT(windowText), 
		WS_POPUP | WS_BORDER, point.x, point.y, 400, 25, mainWin, NULL, (HINSTANCE)lParam, NULL);
	ShowWindow(labelHandler, SW_SHOW);
	newSwitch(switches, hwnd, label);

	return TRUE;
}

void activate(HWND hwnd) {
	if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
	SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	//SwitchToThisWindow(sw->hwnd, TRUE);
	//SetForegroundWindow(sw->hwnd);
	//BringWindowToTop(sw->hwnd);
	//SetActiveWindow(sw->hwnd);
}

void terminate() {
	freeSwitch(switches);
	PostQuitMessage(0);
}
BOOL TWICE = FALSE;
/* Window Procedure for the main window. */
LRESULT CALLBACK mainWinProc(HWND hwnd, UINT msgCode, WPARAM wparam, LPARAM lparam) {
	SWITCH* sw;
	PAINTSTRUCT ps;
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
		if (TWICE) {
			terminate();
			return 0;
		}
	default:
		return DefWindowProc(hwnd, msgCode, wparam, lparam);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE dummy, PSTR lpCmdLine, int nCmdShow) {
	int x, y;
	HWND hwnd;
	MSG msg;
	WNDCLASS class;

	class.style		= 0;
	class.lpfnWndProc	= mainWinProc;
	class.cbClsExtra	= class.cbWndExtra	= 0;
	class.hInstance		= hInstance;
	class.hIcon		= LoadIcon(NULL , IDI_APPLICATION);
	class.hCursor		= LoadCursor(NULL , IDC_ARROW);
	class.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	class.lpszMenuName	= NULL;
	class.lpszClassName	= TEXT("Switcher");

	if (!RegisterClass(&class)) return -1;

	//mainWin = CreateWindowEx(WS_EX_TOOLWINDOW, TEXT("Switcher"), TEXT("rchsch\0"), 
	mainWin = CreateWindow(TEXT("Switcher"), TEXT("rchsch\0"), 
		WS_POPUP | WS_BORDER, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if (mainWin == NULL) return -1;
	
	switches = initSwitchList();
	EnumWindows(EnumWindowsProc , (LPARAM)hInstance);
	if (nSwitches <= 1) {
		terminate();
		return 0;
	}
	if (nSwitches == 2) {
		activate((*switches)->hwnd);
		terminate();
		return 0;
	}

	ShowWindow(mainWin, SW_SHOW);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
