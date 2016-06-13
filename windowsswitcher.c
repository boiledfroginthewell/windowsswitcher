#include<windows.h>
#include<stdio.h>

#include "switch.h"

#define MAX_TITLE 512


int nSwitches = 0;
HWND mainWin;
SWITCH_LIST switches;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	TCHAR windowText[512]; // TODO: LABEL
	LPRECT lprect;
	TCHAR class[10];

	int i;
	for (i = 0; i < sizeof(windowText); i++) {
		windowText[i] = 0;
	}
	for (i = 0; i < sizeof(class); i++) class[i] = 0;
	GetWindowText(hwnd , windowText, sizeof(windowText) - 10);
	if (hwnd == NULL) return TRUE;
	if (!IsWindowVisible(hwnd)) return TRUE;
	if (GetWindow(hwnd, GW_OWNER) != NULL) return TRUE;
	if (windowText[0] == TEXT('\0')) return TRUE;
	GetClassName(hwnd, class, sizeof(class) - 1);
	if (lstrcmp(class, TEXT("Progman")) == 0) return TRUE;
	GetWindowRect(hwnd, lprect);
	if (lprect == NULL) return TRUE;
	// TODO: ex_edge or border
	hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW, TEXT("STATIC"), TEXT(windowText), 
		WS_POPUP | WS_BORDER | WS_VISIBLE, lprect->left, lprect->top, 400, 30, mainWin, NULL, (HINSTANCE)lParam, NULL);
	ShowWindow(hwnd, SW_SHOW);
	//newSwitch(switches, hwnd, 'a');
	return TRUE;
}


//HWND buildWindow(LPCTSTR text) {

/* Window Procedure for the main window. */
LRESULT CALLBACK mainWinProc(HWND hwnd, UINT msgCode, WPARAM wparam, LPARAM lparam) {
	//printf("%d\n", msgCode);
	switch(msgCode) {
	case WM_CHAR:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
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

	mainWin = CreateWindow(TEXT("Switcher"), TEXT("rchsch\0"), 
		WS_POPUP | WS_BORDER, 0, 0, 10, 100, NULL, NULL, hInstance, NULL);
	if (mainWin == NULL) return -1;
	
	// TODO get positon and set
	EnumWindows(EnumWindowsProc , (LPARAM)hInstance);
	ShowWindow(mainWin, SW_SHOW);
	//SetActiveWindow(mainWin);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
