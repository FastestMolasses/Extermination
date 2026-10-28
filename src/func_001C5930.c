// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 69.04% NEARMISS).
// Fixes read from the original instructions:
// - the state byte at +4 is one switch (cases 2 and 3 share one call), and
//   the +5 and +6 sub-states are switches too (+5 case 0 falls into case 1's
//   D_008106B8 check); the old goto chain had stray paths.
// - the area-5 location name passes the palette/glyph table D_00265520 as the
//   last draw argument (the old C passed 0 on both arms).
// - the suppress flag is an if/else on the scratchpad selector held in an int
//   (1, or 2..3 as an unsigned range test).
// - the +0x2A string index adds D_00810701 with `+=`; the first label's x is
//   written as -(width >> 1) + 0x800.
// - callee prototypes match their definitions (func_001CC170 takes a string,
//   func_001CC1E0 takes seven arguments ending in two pointers, returns void).

// Area/location name banner (state machine). arg0 is the task record: +4 state, +5 / +6
// sub-states, +0x28 a 300-frame timer, +0x2A the area-name string index, and
// at +0x1F0 a {current location id, 300-frame timer} pair.
//   state 0: arms the timers, picks the area name index from the per-area
//            table D_00289B40 plus the sub-area D_00810701, records the
//            location id from func_001C5860 and advances.
//   state 1: unless the scratchpad selector 0x70003B8D is 1..3, draws the
//            area name centred on x = 0x800 until the +0x28 timer runs out,
//            then (sub-state +5 == 1) draws the location name
//            D_0026726C[id] centred on x = 0x896 for 300 frames, refreshing
//            the id from func_001C5860 when it changes. D_008106B8 set jumps
//            to state 3.
//   states 2 and 3: func_001AFC10(arg0).

extern volatile unsigned char D_70003B8D;

typedef struct {
    int id;
    short timer;
} Sub;

extern void func_001AFC10(unsigned char *);
extern int func_001C5860(void);
extern int func_001CC170(unsigned char *s);
extern void func_001CC1E0(int a0, int a1, int a2, int a3, int t0, void *t1, void *t2);

extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_008106B8;
extern short D_00289B40[][2];
extern unsigned char *D_002671C0[];
extern unsigned char *D_0026726C[];
extern char D_00265520[];

void func_001C5930(unsigned char *arg0) {
    Sub *sub;
    int flag;
    unsigned char *s;
    int id;
    int v;

    sub = (Sub *)(arg0 + 0x1F0);
    switch (arg0[4]) {
    case 0:
        *(short *)(arg0 + 0x28) = 300;
        *(short *)(arg0 + 0x2A) = D_00289B40[D_00810700][0];
        *(short *)(arg0 + 0x2A) += D_00810701;
        arg0[4]++;
        sub->id = func_001C5860();
        sub->timer = 300;
        break;
    case 1:
        {
            int m = D_70003B8D;
            if (m == 1 || (unsigned int)(m - 2) < 2) flag = 1; else flag = 0;
        }
        switch (arg0[5]) {
        case 0:
            if (!flag) {
                s = D_002671C0[*(short *)(arg0 + 0x2A)];
                func_001CC1E0(1, -(func_001CC170(s) >> 1) + 0x800, 0x7A2, 0xA, 0x14, s, 0);
            }
            if (--*(short *)(arg0 + 0x28) == 0) {
                arg0[5]++;
            }
        case 1:
            if (D_008106B8 != 0) {
                arg0[4] = 3;
            }
            break;
        }
        if (flag) {
            break;
        }
        switch (arg0[6]) {
        case 0:
            id = sub->id;
            if (id == 0) {
                sub->timer = 300;
                arg0[6]++;
            } else if (arg0[5] == 1) {
                s = D_0026726C[id];
                if (id == 5) {
                    func_001CC1E0(1, 0x896 - (func_001CC170(s) >> 1), 0x7A2, 0xA, 0x14, s, D_00265520);
                } else {
                    func_001CC1E0(1, 0x896 - (func_001CC170(s) >> 1), 0x7A2, 0xA, 0x14, s, 0);
                }
                if (--sub->timer == 0) {
                    sub->timer = 300;
                    arg0[6]++;
                } else {
                    v = func_001C5860();
                    if (sub->id != v) {
                        sub->id = v;
                        sub->timer = 300;
                    }
                }
            }
            break;
        case 1:
            v = func_001C5860();
            if (sub->id != v) {
                sub->id = v;
                arg0[6] = 0;
            }
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
