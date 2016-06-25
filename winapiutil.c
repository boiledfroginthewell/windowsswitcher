#include<windows.h>
#include<stdio.h>
#include "winapiutil.h"

/* 
 * Calculates the RECT of a maximized/normal/minimized/aero-snapped window in the screen coordinates. 
 * Different from the workspace coordinates, which GetWindowPlacement returns, 
 * the screen coordinates are not depends on the location of the taskbar. The screen coordinates are 
 * used for many functions such as MoveWindow, SetWindowPos, etc.
 *
 * If hwnd is NULL, the RECT of the workarea (i.e., the primary display without the taskbar) is 
 * returned in the screen coordinates.
 */
void getWindowGeo(HWND hwnd, RECT *rect) {
	WINDOWPLACEMENT place;
	RECT workarea;

	if (rect == NULL) return;

	// RECT of workarea
	if (hwnd == NULL) {
		SystemParametersInfo(SPI_GETWORKAREA, 0, rect, 0);
		return;
	}

	// RECT of windows
	if (!IsIconic(hwnd)) {
		GetWindowRect(hwnd, rect);
	} else {
		GetWindowPlacement(hwnd, &place);
		if ((place.flags & WPF_RESTORETOMAXIMIZED) || (place.showCmd == SW_MAXIMIZE)) {
			// Maximized window
			rect->left = place.ptMaxPosition.x;
			rect->top = place.ptMaxPosition.y;
		} else {
			rect->left = place.rcNormalPosition.left;
			rect->top = place.rcNormalPosition.top ;
		}
		// FIXME: Cannot determine whether a minimized window is to be aero-snapped when it is restored from minimization.

		// Convert the workspace coordinates into the screen coordinates
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);
		if (1 < rect->left || 1 < rect->top) return; // taskbar is not on left or top.
		rect->top += workarea.top;
		rect->left += workarea.left; 
		rect->right += workarea.right;
		rect->bottom += workarea.top;
	}
	return;
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
	
#ifdef ICON_SMALL2
	icon = SendMessage(hwnd, WM_GETICON, ICON_SMALL2, 0);
	if (icon != (ULONG_PTR) NULL) return (HICON) icon;
#endif
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

/* Identifies whether the window is shown in the taskbar */
BOOL isInTaskbar(HWND hwnd) {
	LONG_PTR exStyle;

	if (hwnd == NULL) return FALSE;
	if (!IsWindowVisible(hwnd)) return FALSE;
	if (GetWindow(hwnd, GW_OWNER) != NULL) return FALSE;
	exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	if (exStyle & WS_EX_TOOLWINDOW) return FALSE;
	//if (exStyle & WS_EX_NOREDIRECTIONBITMAP) return FALSE;

	return TRUE;
}
