#include<windows.h>
#include "winapiutil.h"

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


void activate(HWND hwnd) {
	if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
	SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	//SwitchToThisWindow(hwnd, TRUE);
	//SetForegroundWindow(hwnd);
	//BringWindowToTop(sw->hwnd);
	//SetActiveWindow(sw->hwnd);
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
