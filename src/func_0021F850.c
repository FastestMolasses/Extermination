// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// SEMANTICS: per-frame update of a scripted actor/cutscene entity `e`.
// Dispatches on the unsigned-byte step counter at e+6 through a 9-entry jump
// table (jtbl_00273850); out-of-range steps fall straight through to the tail.
// Step 0  : kick off func_001FBD50(e, 0x150, 300.0) + func_001749A0(e, 0x2E, 6.0),
//           clear e+7, raise the global flag D_008106BC, zero the timers at
//           e+0x38 / e+0x21C / e+0x2EC and advance.
// Step 1  : wait until bit 0x8000 of the flag word at e+0x200 clears, then advance.
// Step 2  : once the float at e+0x3C has dropped to <= 35.0, advance and fire
//           func_001FBD50(e, 0x12F, 300.0) + func_001B61C0(0, 0xD0, 10, 1);
//           FALLS THROUGH into step 3 in the same frame.
// Step 3  : if bit 0x1000 of e+0x200 is set, branch on the float at e+0x228 and
//           the global D_008106F1: either advance by 1 and call func_0021C120(e)
//           (setting e+0x302 = 1) or skip ahead by 3 (e+0x302 = 0).
//           Otherwise accumulate elapsed time (global clock **D_00275B40 + 8)
//           into e+0x38 / e+0x21C and call func_00178B90(e, 1).
// Step 4  : advance and arm the 0x10-frame countdown at e+0x28 once
//           func_0021C190() reports ready.
// Step 5  : run the e+0x28 countdown down through 0, then advance.
// Step 6  : advance, fire func_001749A0(e, 0x24, 8.0), reset the timers.
// Step 7  : wait for bit 0x8000 of e+0x200 to clear, then advance and clear e+0x1F1.
// Step 8  : if bit 0x1000 of e+0x200 is set, tear down (clear e+0xF, e+0x224,
//           e+0x22C, e+0x25C and D_008106BC, set the 0x3C timer at e+0x20E) and
//           call func_0017C540(e); otherwise keep accumulating elapsed time and,
//           when e+0x302 is set, store 0.4f into e+0x204.
// Tail (all paths): func_00179880(e, e + 0x2EC) then func_00175900(e, 1).
// NOTE: the 0-stores into the float fields e+0x38 / e+0x21C are written as int
// stores because 0.0f and (int)0 share the same bit pattern and the original
// emits `sw zero`.

extern char **D_00275B40;
extern unsigned char D_008106BC[8];
extern unsigned char D_008106F1[8];

extern void func_001749A0(char *, int, int, float);
extern void func_00175900(char *, int);
extern void func_00178B90(char *, int);
extern void func_00179880(char *, char *);
extern void func_0017C540(char *);
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(char *, int, int, float);
extern void func_0021C120(char *);
extern int func_0021C190(void);

void func_0021F850(char *e) {
    int st;
    int t;

    st = *(unsigned char *)(e + 6);
    switch (st) {
    case 0:
        func_001FBD50(e, 0x150, 0, 300.0f);
        *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
        *(char *)(e + 7) = 0;
        D_008106BC[0] = 1;
        func_001749A0(e, 0x2E, 0, 6.0f);
        *(int *)(e + 0x38) = 0;
        *(int *)(e + 0x21C) = 0;
        *(int *)(e + 0x2EC) = 0;
        break;
    case 1:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            *(unsigned char *)(e + 6) = st + 1;
        }
        break;
    case 2:
        if (*(float *)(e + 0x3C) <= 35.0f) {
            *(unsigned char *)(e + 6) = st + 1;
            func_001FBD50(e, 0x12F, 0, 300.0f);
            func_001B61C0(0, 0xD0, 0xA, 1);
        }
        /* fallthrough */
    case 3:
        if (*(int *)(e + 0x200) & 0x1000) {
            if (*(float *)(e + 0x228) >= 100.0f && D_008106F1[0] != 0) {
                *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
                func_0021C120(e);
                *(unsigned char *)(e + 0x302) = 1;
            } else {
                *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 3;
                *(unsigned char *)(e + 0x302) = 0;
            }
        } else {
            *(float *)(e + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(e, 1);
        }
        break;
    case 4:
        if (func_0021C190() != 0) {
            *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
            *(short *)(e + 0x28) = 0x10;
        }
        break;
    case 5:
        t = *(short *)(e + 0x28);
        *(short *)(e + 0x28) = t - 1;
        if (t == 0) {
            *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
        }
        break;
    case 6:
        *(unsigned char *)(e + 6) = st + 1;
        func_001749A0(e, 0x24, 0, 8.0f);
        *(int *)(e + 0x38) = 0;
        *(int *)(e + 0x21C) = 0;
        break;
    case 7:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            *(unsigned char *)(e + 6) = st + 1;
            *(char *)(e + 0x1F1) = 0;
        }
        break;
    case 8:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(char *)(e + 0xF) = 0;
            *(int *)(e + 0x224) = 0;
            *(int *)(e + 0x22C) = 0;
            D_008106BC[0] = 0;
            *(short *)(e + 0x20E) = 0x3C;
            *(char *)(e + 0x25C) = 0;
            func_0017C540(e);
        } else {
            *(float *)(e + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(e, 1);
            if (*(unsigned char *)(e + 0x302) != 0) {
                *(int *)(e + 0x204) = 0x3ECCCCCD;
            }
        }
        break;
    }
    func_00179880(e, e + 0x2EC);
    func_00175900(e, 1);
}
