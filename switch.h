
#define LABELS "aoeui:,.py;qjkx"

typedef struct S_SWITCH{
	HWND hwnd;
	char label;
	struct S_SWITCH *next;
} SWITCH;
typedef SWITCH **SWITCH_LIST;


int newSwitch(SWITCH_LIST tgt, HWND hwnd, char label);
void freeSwitch(SWITCH_LIST start);
