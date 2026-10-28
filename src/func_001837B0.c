// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Player state worker for state byte e[5] == 1 (dispatched from
// func_0015B530), sub-state e[6]:
//   0: advance to 1, clear e[7], call func_0017B5C0(e), then fall into 1.
//   1: once contact flag 0x8000 of e+0x200 is clear, advance, clear e[0x25C]
//      and set e[0x1F0] = e[0x1F1] = 1.
//   2: func_0017BC40, func_0017C030, func_00178B90(e, 1); when e[0x1F0] drops
//      to 0, return to state 0 (e[5] = e[6] = 0).
// Every frame afterwards: e+0xB4 -= e+0x38 * sinf(e+0x9C), then e+0xB4 -= 0.2,
// then func_00175900(e, 1) and func_001796C0(e).
extern float func_0011E2A8(float x);
extern void func_0017B5C0(unsigned char *e);
extern void func_0017BC40(unsigned char *e);
extern void func_0017C030(unsigned char *e);
extern void func_00178B90(unsigned char *e, int collide);
extern unsigned char func_00175900(unsigned char *e, int probe_around);
extern void func_001796C0(unsigned char *e);

void func_001837B0(unsigned char *e) {
    switch (e[6]) {
    case 0:
        e[6]++;
        e[7] = 0;
        func_0017B5C0(e);
    case 1:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            e[6]++;
            e[0x25C] = 0;
            e[0x1F0] = 1;
            e[0x1F1] = 1;
        }
        break;
    case 2:
        func_0017BC40(e);
        func_0017C030(e);
        func_00178B90(e, 1);
        if (e[0x1F0] == 0) {
            e[5] = 0;
            e[6] = 0;
        }
        break;
    }
    *(float *)(e + 0xB4) -= *(float *)(e + 0x38) * func_0011E2A8(*(float *)(e + 0x9C));
    *(float *)(e + 0xB4) += -0.2f;
    func_00175900(e, 1);
    func_001796C0(e);
}
