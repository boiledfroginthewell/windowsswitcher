#ifndef __H_WINAPIUTIL_
#define __H_WINAPIUTIL_ 1

void getWindowGeo(HWND, RECT*); 
void activate(HWND); 
HICON getWindowIcon(HWND);
BOOL isInTaskbar(HWND); 

#endif
