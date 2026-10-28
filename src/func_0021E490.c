// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick handler for an actor sub-state (state byte p+6), called from the
// dispatcher func_0015B770 for state 2/24.
// 0: sound 0x153 (func_001FBD50, range 300), pad rumble func_001B61C0(0,
//    0xC0, 5, 1), advance, clear p+7, clip 0x1F (0x57 when p[0x236] is set)
//    with blend 1.0, clear p+0x2EC.
// 1: when p+0x3C <= 22.0 advance and call func_0021C120(p).
// 2: advance when func_0021C190(p) says so, else p+0x204 = 0.1.
// 3: once the clip finished (bit 0x1000 of p+0x200): timer p+0x20E = 0x3C,
//    then func_0021D530(p) in state 5 == 0x18, else clear p[0x25C] and
//    func_0017C540(p); while the clip runs p+0x204 = 0.25.
// Every tick then runs func_001764E0(p), func_00179880(p, p+0x2EC) and
// func_00175900(p, 1).

extern void func_001749A0(char *a0, int a1, int a2, float f12);
extern int func_00175900(char *p, int a);
extern void func_001764E0(char *p);
extern void func_00179880(char *a0, float *a1);
extern void func_0017C540(char *p);
extern void func_001B61C0(int a0, int a1, int a2, int a3);
extern void func_001FBD50(char *a0, int a1, int a2, float f12);
extern void func_0021C120(char *p);
extern int func_0021C190(char *p);
extern void func_0021D530(char *p);

void func_0021E490(char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        func_001FBD50(arg0, 0x153, 0, 300.0f);
        func_001B61C0(0, 0xC0, 5, 1);
        *(unsigned char *)(arg0 + 6) += 1;
        *(char *)(arg0 + 7) = 0;
        if (*(unsigned char *)(arg0 + 0x236) == 0) {
            func_001749A0(arg0, 0x1F, 0, 1.0f);
        } else {
            func_001749A0(arg0, 0x57, 0, 1.0f);
        }
        *(int *)(arg0 + 0x2EC) = 0;
        break;
    case 1:
        if (*(float *)(arg0 + 0x3C) <= 22.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_0021C120(arg0);
        }
        break;
    case 2:
        if (func_0021C190(arg0) != 0) {
            *(unsigned char *)(arg0 + 6) += 1;
        } else {
            *(float *)(arg0 + 0x204) = 0.1f;
        }
        break;
    case 3:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(short *)(arg0 + 0x20E) = 0x3C;
            if (*(unsigned char *)(arg0 + 5) == 0x18) {
                func_0021D530(arg0);
            } else {
                *(char *)(arg0 + 0x25C) = 0;
                func_0017C540(arg0);
            }
        } else {
            *(float *)(arg0 + 0x204) = 0.25f;
        }
        break;
    }
    func_001764E0(arg0);
    func_00179880(arg0, (float *)(arg0 + 0x2EC));
    func_00175900(arg0, 1);
}
