

typedef struct S_SWITCH{
	HWND hwnd;
	char label;
	struct S_SWITCH *next;
} SWITCH;
typedef SWITCH **SWITCH_LIST;


SWITCH_LIST initSwitchList();
int newSwitch(SWITCH_LIST tgt, HWND hwnd, char label);
void freeSwitch(SWITCH_LIST start);
SWITCH* findSwitch(SWITCH_LIST list, char label);
