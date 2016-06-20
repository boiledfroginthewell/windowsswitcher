

typedef struct S_SWITCH{
	HWND hwnd;
	HWND labelHandle;
	char label;
	struct S_SWITCH *next;
} SWITCH;
typedef SWITCH **SWITCH_LIST;


SWITCH_LIST initSwitchList();
int newSwitch(SWITCH_LIST tgt, HWND hwnd, HWND labelHandle, char label);
void freeSwitch(SWITCH_LIST start);
SWITCH* findSwitch(SWITCH_LIST list, char label);
