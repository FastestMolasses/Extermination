// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Per-state actor driver dispatched on the state byte at arg0+3 (states 0/1/2).
// State 0: bumps the state to 1. State 1: fires effect variant 5 via
// func_001FCBD0(0x3C,0x9B,7,5,0x70808080) and, once input flag 0x40 is set,
// advances the state to 2. State 2: fires variant 7 and, on flag 0x40, runs
// func_0020CD40()+func_001FE8D0() and returns 1. After the dispatch it handles
// the shared input/event flags in D_00810E74: bit 0x20 -> func_0020CD60+stop,
// return 1; bit 0x10 -> same, return 2. Then if func_001FE9A0 is ready it steps
// the clip via func_001FECB0(*(0x48),*(0x4C),arg0+0x58,0,0); if the shared busy
// flag D_00275C58 is set it returns 1, else maps the step result: 0 -> (return 0
// if arg0+0x58==2 else 1), -1 -> 0, otherwise 1.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 91.9%. Keys: func_001FCBD0 is a 5-arg call (5th = packed 0x70808080 in $t0);
// func_001FECB0 is 5-arg (trailing two zeros in a3/$t0); func_0020CD60 takes no
// argument (the a0=1 the target sets is the hoisted func_001FE9A0 first arg);
// D_00810E74 is over-declared u16[8] to force absolute addressing while
// D_00275C58 stays gp-relative under -sdatathreshold 8; and the final state-0
// result is written `if(*0x58 != 2) return 1; return 0;` so 2.3.3 emits the
// target's beq-with-v0=0-in-slot return shape. objdiff 100.0 vs expected.
extern void func_001FCBD0(int, int, int, int, int);
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, char *, char *);
extern int func_001FECB0(int, int, char *, int, int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern int D_00275C58;
extern unsigned short D_00810E74[8];

int func_00226B90(char *arg0) {
    int v;

    switch (*(unsigned char *)(arg0 + 3)) {
    case 0:
        *(unsigned char *)(arg0 + 3) = 1;
        break;
    case 1:
        func_001FCBD0(0x3C, 0x9B, 7, 5, 0x70808080);
        if (D_00810E74[0] & 0x40) {
            func_0020CD40();
            *(unsigned char *)(arg0 + 3) = 2;
        }
        break;
    case 2:
        func_001FCBD0(0x3C, 0x9B, 7, 7, 0x70808080);
        if (D_00810E74[0] & 0x40) {
            func_0020CD40();
            func_001FE8D0();
            return 1;
        }
        break;
    }

    if (D_00810E74[0] & 0x20) {
        func_0020CD60();
        func_001FE8D0();
        return 1;
    }
    if (D_00810E74[0] & 0x10) {
        func_0020CD60();
        func_001FE8D0();
        return 2;
    }
    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }
    v = func_001FECB0(*(int *)(arg0 + 0x48), *(int *)(arg0 + 0x4C), arg0 + 0x58, 0, 0);
    if (D_00275C58 != 0) {
        return 1;
    }
    if (v == 0) {
        if (*(int *)(arg0 + 0x58) != 2) {
            return 1;
        }
        return 0;
    }
    if (v == -1) {
        return 0;
    }
    return 1;
}
