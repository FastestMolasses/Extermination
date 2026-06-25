// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Actor per-frame state-machine update (object pointer in arg0). Plays sound
// effects keyed on the substate byte (+0x4) via func_001FCBD0(...,0x70808080)
// (the 0x70808080 5th arg is a packed color/flag passed in $t0), then gates on
// func_001FE9A0(1,&+0x50,&+0x54). On the global input/event flags D_00810E74:
// bit 0x20 -> func_0020CD60, clear +0x9, return 1; bit 0x10 -> func_0020CD60,
// return 2. Otherwise advances the +0x4 state, handling the abort/2000/8000
// flag bits, then drives the shared playback step func_001FECB0(+0x48,+0x4C,
// &+0x58,0,0) whose result maps to 0/1 (via *(int*)(+0x58)==2) or -1->0/else 1.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202. The
// func_001FECB0 tail only matches CW's branch lowering when written r==0
// fall-through first and the r==-1 guard as `if(r==-1)return 0; return 1;`;
// 991202 mis-lowers that tail (caps ~90.5%). D_00810E74 is read fresh at each
// bit test (not cached) to avoid a spurious andi 0xffff zero-extend. Verified
// objdiff 100.0 vs build/expected/func_00227DE0.o.
extern int func_001FCBD0(int, int, int, int, unsigned int);
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, char *, char *);
extern int func_001FECB0(int, int, char *, int, int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(void);
extern void func_002256E0(void);
extern void func_00225700(void);
extern unsigned short D_00810E74;

int func_00227DE0(char *arg0) {
    unsigned char v1;
    int r;

    v1 = *(unsigned char *)(arg0 + 4);
    switch (v1) {
    case 0:
        break;
    case 1:
        func_001FCBD0(0x3C, 0x9B, 7, 0xD, 0x70808080);
        break;
    case 2:
        func_001FCBD0(0x3C, 0x9B, 7, 0x1F, 0x70808080);
        func_001FCBD0(0x8C, 0xC3, 7, 2, 0x70808080);
        v1 = *(unsigned char *)(arg0 + 9);
        switch (v1) {
        case 0:
            func_00225700();
            break;
        case 1:
            func_002256E0();
            break;
        }
        break;
    }
    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }
    if (D_00810E74 & 0x20) {
        func_0020CD60();
        *(unsigned char *)(arg0 + 9) = 0;
        func_001FE8D0();
        return 1;
    }
    if (D_00810E74 & 0x10) {
        func_0020CD60();
        *(unsigned char *)(arg0 + 9) = 0;
        func_001FE8D0();
        return 2;
    }
    v1 = *(unsigned char *)(arg0 + 4);
    switch (v1) {
    case 0:
        *(unsigned char *)(arg0 + 4) = 1;
        *(unsigned char *)(arg0 + 9) = 0;
        break;
    case 1:
        if (D_00810E74 & 0x40) {
            func_0020CD40();
            *(unsigned char *)(arg0 + 4) = 2;
        }
        break;
    case 2:
        if (D_00810E74 & 0x40) {
            func_0020CD40();
            func_001FE8D0();
            if (*(unsigned char *)(arg0 + 9) != 0) {
                *(unsigned char *)(arg0 + 9) = 0;
                return 3;
            }
            *(unsigned char *)(arg0 + 9) = 0;
            return 1;
        }
        if ((D_00810E74 & 0x2000) && *(unsigned char *)(arg0 + 9) != 0) {
            *(unsigned char *)(arg0 + 9) = 0;
            func_0020CDA0();
        }
        if ((D_00810E74 & 0x8000) && *(unsigned char *)(arg0 + 9) == 0) {
            *(unsigned char *)(arg0 + 9) = 1;
            func_0020CDA0();
        }
        break;
    }
    r = func_001FECB0(*(int *)(arg0 + 0x48), *(int *)(arg0 + 0x4C), arg0 + 0x58, 0, 0);
    if (r == 0) {
        if (*(int *)(arg0 + 0x58) != 2) {
            return 1;
        }
        return 0;
    }
    if (r == -1) {
        return 0;
    }
    return 1;
}
