
#define LABELS "aoeui:,.py;qjkx"

typedef SWITCH** SWITCH_LIST;
typedef struct {
	HWND hwnd;
	char label;
	HWND* next;
} SWITCH;


int newSwitch(SWITCH_LIST tgt, HWND hwnd, char label);
void free(SWITCH_LIST start);
