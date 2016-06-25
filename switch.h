#ifndef __H_SWITCH_
#define __H_SWITCH_ 1

#define SUB_WIN_CLASS_NAME TEXT("Switch")

#define MAX_SWITCH_TEXT 256
#define LABEL_OFFSET 3
#define LABELS "gfdsatrewqvcx12345hjkl;yuiopnm,.7890"

/* Padding for a switch window */
#define PADDING 3
#define PADDING_L PADDING
#define PADDING_R (PADDING + 3)
#define PADDING_T PADDING
#define PADDING_B PADDING
/* Space between the icon and texts */
#define ICON_SPACING 10

extern HWND mainWin;


typedef struct S_SWITCH{
	HWND hwnd;
	HWND labelHandle;
	char label;
	struct S_SWITCH *next;
} SWITCH;
typedef SWITCH **SWITCH_LIST;

BOOL makeSubWinClass(HINSTANCE); 
LRESULT CALLBACK subWinProc(HWND, UINT, WPARAM, LPARAM);

SWITCH_LIST initSwitchList(void);
int newSwitch(SWITCH_LIST, HWND, HINSTANCE);
void freeSwitch(SWITCH_LIST);
int countSwitches(); 
SWITCH* findSwitch(SWITCH_LIST, char);

#endif
