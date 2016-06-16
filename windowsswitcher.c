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
	TCHAR windowText[512 + LABEL_OFFSET]; // TODO: LABEL
	RECT rect;
	TCHAR class[10];
	char label;

	int i;
	for (i = 0; i < sizeof(windowText); i++) {
		windowText[i] = 0;
	}
	for (i = 0; i < sizeof(class); i++) class[i] = 0;
	GetWindowText(hwnd , windowText + LABEL_OFFSET, sizeof(windowText) - 10 - LABEL_OFFSET);
	if (hwnd == NULL) return TRUE;
	if (!IsWindowVisible(hwnd)) return TRUE;
	//if (GetWindow(hwnd, GW_OWNER) != NULL) return TRUE;
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
	GetWindowRect(hwnd, &rect);
	
	// TODO: ex_edge or border
	hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW, TEXT("STATIC"), TEXT(windowText), 
		WS_POPUP | WS_BORDER, rect.left, rect.top, 400, 30, mainWin, NULL, (HINSTANCE)lParam, NULL);
	ShowWindow(hwnd, SW_SHOW);
	newSwitch(switches, hwnd, label);
	return TRUE;
}

void terminate() {
	freeSwitch(switches);
	PostQuitMessage(0);
}

/* Window Procedure for the main window. */
LRESULT CALLBACK mainWinProc(HWND hwnd, UINT msgCode, WPARAM wparam, LPARAM lparam) {
	SWITCH* sw;
	switch(msgCode) {
	case WM_CHAR:
		if ((sw = findSwitch(switches, ((TCHAR) wparam))) == NULL) {
			terminate();
			return 0;
		}
		Sleep(2000);
		printf("iji\n");fflush(stdout);
//		SystemParametersInfo(
//		ShowWindow(sw->hwnd, SW_RESTORE);
		ShowWindow(sw->hwnd, SW_SHOW);
	SwitchToThisWindow(sw->hwnd, TRUE);
		SetForegroundWindow(sw->hwnd);
 BringWindowToTop(sw->hwnd);
//		SetActiveWindow(sw->hwnd);
//SetWindowPos(sw->hwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
SetWindowPos(sw->hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
		//MessageBox(NULL , TEXT("Kitty on your lap"), TEXT("メッセージボックス") , MB_OK);
	
	SwitchToThisWindow(sw->hwnd, TRUE);
		UpdateWindow(sw->hwnd);
	
	SwitchToThisWindow(sw->hwnd, TRUE);
	
	 
//First, we set the window as Topmost window in the Z order, but without the focus
SetWindowPos(sw->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
 
//Set the window as non-topmost window, to be sure it will not be always on top
SetWindowPos(sw->hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
 
		terminate();
		return 0;
	case WM_DESTROY:
		terminate();
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
	setvbuf(stdout, 0, _IONBF, 0);
	printf("IJIdaf\n");
	fflush(stdout);

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
	
	switches = initSwitchList();
	// TODO get positon and set
	EnumWindows(EnumWindowsProc , (LPARAM)hInstance);
	ShowWindow(mainWin, SW_SHOW);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
