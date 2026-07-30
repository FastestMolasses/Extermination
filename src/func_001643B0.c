// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: per-frame step of an entity behaviour driven by the state byte at
// e[7] (jump table jtbl_0026D640, 6 states, default = fall straight through to
// the shared tail).
//   0: bump state, start animation 0x75 with blend 1.0, raise the e[0x302] flag
//      and clear the int at e+0x2EC.
//   1: gated on the float at e+0x3C <= 40.0 -- fires func_001B61C0(1,0xEE,0x3C,1),
//      clamps the float at e+0x220 down to 60.0, calls func_0021C120(e), bumps state.
//   2: poll func_0021C190(e); on success bump state and clear the e[0x302] flag,
//      otherwise zero the float at e+0x204.
//   3: if bit 0x1000 of the int at e+0x200 is set bump state (computed from the
//      switch value still live in a register), else write 0.25 to e+0x204.
//   4: func_00174AC0(e,0); if the byte at e+0x23F > 1 bump state and
//      func_0017C440(e,0), else clear e[0x25C] and func_0017C540(e).
//   5: func_00178B90(e,0); unless bit 0x8000 of e+0x200 is set, func_0017C540(e).
// Shared tail: func_001764E0(e), then either (flag e[0x302] set)
// func_00179880(e, e+0x2EC) + func_00175900(e,1), or (clear) decay the float at
// e+0xB4 by 0.2, func_00175900(e,1) and func_001796C0(e).

extern int  func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern void func_00174AC0(unsigned char *e, int n);
extern int  func_00175900(unsigned char *e, int f);
extern void func_001764E0(unsigned char *e);
extern void func_00178B90(unsigned char *e, int n);
extern void func_001796C0(unsigned char *e);
extern void func_00179880(unsigned char *e, unsigned char *p);
extern void func_0017C440(unsigned char *e, int f);
extern void func_0017C540(unsigned char *e);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_0021C120(unsigned char *e);
extern int  func_0021C190(unsigned char *e);

void func_001643B0(unsigned char *e) {
    unsigned char state;

    state = e[7];
    switch (state) {
    case 0:
        e[7]++;
        func_001749A0(e, 0x75, 0, 1.0f);
        e[0x302] = 1;
        *(int *)(e + 0x2EC) = 0;
        break;
    case 1:
        if (*(float *)(e + 0x3C) <= 40.0f) {
            func_001B61C0(1, 0xEE, 0x3C, 1);
            if (!(*(float *)(e + 0x220) <= 60.0f)) {
                *(float *)(e + 0x220) = 60.0f;
            }
            func_0021C120(e);
            e[7] = e[7] + 1;
        }
        break;
    case 2:
        if (func_0021C190(e) != 0) {
            e[7] = e[7] + 1;
            e[0x302] = 0;
        } else {
            *(float *)(e + 0x204) = 0.0f;
        }
        break;
    case 3:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[7] = state + 1;
        } else {
            *(float *)(e + 0x204) = 0.25f;
        }
        break;
    case 4:
        func_00174AC0(e, 0);
        if (e[0x23F] > 1) {
            e[7] = e[7] + 1;
            func_0017C440(e, 0);
        } else {
            e[0x25C] = 0;
            func_0017C540(e);
        }
        break;
    case 5:
        func_00178B90(e, 0);
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            func_0017C540(e);
        }
        break;
    default:
        break;
    }
    func_001764E0(e);
    if (e[0x302] != 0) {
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
    } else {
        *(float *)(e + 0xB4) += -0.2f;
        func_00175900(e, 1);
        func_001796C0(e);
    }
}
