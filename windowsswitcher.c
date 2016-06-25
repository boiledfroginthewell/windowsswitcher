#include<stdio.h>
#include<windows.h>

#include "switch.h"
#include "winapiutil.h"

#define SNAP 100
#define SNAP_X SNAP
#define SNAP_Y SNAP

LRESULT CALLBACK mainWinProc(HWND hwnd, UINT msgCode, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam); 
void manageSwitches(SWITCH_LIST switches); 
void terminate(void); 

HWND mainWin;
SWITCH_LIST switches;
BOOL isTerminated = FALSE;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE dummy, PSTR lpCmdLine, int nCmdShow) {
	HWND hwnd;
	MSG msg;
	WNDCLASS class;
	int nSwitches;

	/* Create the main window class */
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

	/* Create the switch window class */
	if (!makeSubWinClass(hInstance)) return -1;

	/* Create the main window, which accepts key inputs */
	mainWin = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_LEFT, TEXT("Switcher"), 0, 
		WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if (mainWin == NULL) return -1;
	
	/* Generate switch windows */
	switches = initSwitchList();
	EnumWindows(EnumWindowsProc , (LPARAM)hInstance);

	/* Special handling when #windows is small */
	nSwitches = countSwitches();
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


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	if (!isInTaskbar(hwnd)) return TRUE;
	newSwitch(switches, hwnd, (HINSTANCE) lParam);
	return TRUE;
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
		if (lstrcmp(name, SUB_WIN_CLASS_NAME) != 0) {
			terminate();
			return 0;
		}
	default:
		return DefWindowProc(hwnd, msgCode, wparam, lparam);
	}
}

void terminate() {
	if (isTerminated) return;

	freeSwitch(switches);
	switches = NULL;
	isTerminated = TRUE;
	PostQuitMessage(0);
}

