// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x008251E0 (splat/link name 008251A0; overlay code is
// linked 0x40 below where it runs), 0x204 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Role: state 0: state 3 when D_00810792 is set, else state 4. State 4:
// state 3 once D_00810792 is set; otherwise, when the player (D_00810350
// x, D_00810358 z) is inside x 312..336 / z 413..427 or x 319..336 / z
// 390..427 and D_008102B5 < 2, it sets +0xB = 4, starts script 0x8292C0 and
// goes to state 1. State 1: at the script end D_00810792 = 1 and state 3.
// State 3/other: func_001AFC10.
extern unsigned char D_00810792;
extern unsigned char D_008102B5;
extern float D_00810350;
extern float D_00810358;
extern char D_overlay_AREA11_008292C0[];
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA11_008251A0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    float x;
    int mode;
    switch (self[4]) {
    case 0:
        if (D_00810792 != 0) {
            self[4] = 3;
        } else {
            self[4] = 4;
        }
        break;
    case 4:
        if (D_00810792 != 0) {
            self[4] = 3;
            break;
        }
        x = D_00810350;
        if ((x > 312.0f && x < 336.0f && D_00810358 > 413.0f && D_00810358 < 427.0f) ||
            (x > 319.0f && x < 336.0f && D_00810358 > 390.0f && D_00810358 < 427.0f)) {
            mode = D_008102B5;
            if (mode < 2) {
                self[0xB] = 4;
                func_001BA1A0(talk, D_overlay_AREA11_008292C0);
                self[4] = 1;
            }
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            D_00810792 = 1;
            self[4] = 3;
        }
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
