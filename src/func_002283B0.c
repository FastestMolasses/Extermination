// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Per-state stream/clip driver, gated on func_001FE9A0 readiness (a0=1, the
// s0+0x50/0x54 channel pair). If the gate returns 0 the func returns 0.
// Otherwise dispatch on the state byte at s0+2 (states 0/1/2; sibling of the
// matched func_00226B90/func_0022A020 family):
//   state 0: step func_001FEC20(*(s0+0x48), *(s0+0x4C), &D_002738E0, 0, 1,
//            &D_00821500) -- a SIX-arg call; under the EABI arg5 lands in $8 and
//            arg6 in $9, and mwcc CSEs the literal 1 with the state-dispatch
//            compare constant, which is why both share $8. If it reports <0 fall
//            to the shared busy epilogue; if it reports 0 run
//            func_00121A28(s0+0x6C, 0, 8) and return 1; otherwise advance to 1.
//   state 1: r = sub_BASCUS_97112_DS00_00_EX_DATA_00_3(s0); switch(r): 3 -> return 3,
//            2 -> return 2, 1 -> advance state to 2, 0 -> fall through. The 2/3 arms
//            use the target's beql branch-likely with the return value (paddub) in
//            the delay slot -- reproduced by the ascending-case switch.
//   state 2: return 1.
// Shared epilogue: return D_00275C58 ? 2 : 0  (the movz v0,zero,D_00275C58 idiom).
extern int func_001FE9A0(int, char *, char *);
extern int func_001FEC20(int, int, int, int, int, int);
extern void func_00121A28(int, int, int);
extern int sub_BASCUS_97112_DS00_00_EX_DATA_00_3(int);
extern int D_00275C58;
extern int D_002738E0[];
extern int D_00821500[];

int func_002283B0(char *arg0) {
    char *s0;
    int r;

    s0 = arg0;
    if (func_001FE9A0(1, s0 + 0x50, s0 + 0x54) == 0) {
        return 0;
    }
    switch (*(unsigned char *)(s0 + 2)) {
    case 0:
        r = func_001FEC20(*(int *)(s0 + 0x48), *(int *)(s0 + 0x4C), (int)D_002738E0, 0, 1, (int)D_00821500);
        if (r < 0) {
            break;
        }
        if (r == 0) {
            func_00121A28((int)(s0 + 0x6C), 0, 8);
            return 1;
        }
        *(unsigned char *)(s0 + 2) = 1;
        break;
    case 1:
        r = sub_BASCUS_97112_DS00_00_EX_DATA_00_3((int)s0);
        switch (r) {
        case 0:
            break;
        case 1:
            *(unsigned char *)(s0 + 2) = 2;
            break;
        case 2:
            return 2;
        case 3:
            return 3;
        }
        break;
    case 2:
        return 1;
    }
    if (D_00275C58 != 0) {
        return 2;
    }
    return 0;
}
