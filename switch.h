#ifndef __H_SWITCH_
#define __H_SWITCH_ 1

#define SUB_WIN_CLASS_NAME TEXT("Switch")

#define MAX_SWITCH_TEXT 256
#define LABEL_OFFSET 3
#define LABELS "iueoap.,:jq234dhtnsgcrmw89"

#define Y_MOVE_WEIGHT (4.0 / 3.0)
#define X_MARGIN 4
#define Y_MARGIN 0

/* Padding for a switch window */
#define PADDING 3
#define PADDING_L PADDING
#define PADDING_R (PADDING + 3)
#define PADDING_T PADDING
#define PADDING_B PADDING
/* Space between the icon and texts */
#define ICON_SPACING 10

typedef struct S_SWITCH{
	HWND hwnd;
	HWND labelHandle;
	char label;
	struct S_SWITCH *next;
} SWITCH;
typedef SWITCH **SWITCH_LIST;

BOOL makeSubWinClass(HINSTANCE); 
LRESULT CALLBACK subWinProc(HWND, UINT, WPARAM, LPARAM);
void manageSwitches(SWITCH_LIST);

SWITCH_LIST initSwitchList(void);
int newSwitch(SWITCH_LIST, HWND, HWND, HINSTANCE);
void freeSwitch(SWITCH_LIST);
int countSwitches(); 
SWITCH* findSwitch(SWITCH_LIST, char);

#endif
