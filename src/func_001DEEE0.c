// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State machine on the byte at p[0]. p[8] feeds the predicate func_001D2910
// (advance test); p[0x1C] is an 8-step counter clamped to [0, p[4]]; p+0x10 is
// handed to func_001DF110 (per-frame update) in the active states.
//   0: init counter, advance to 1 if test passes else 3
//   1: ramp counter up by 8 (clamp to p[4]); if test fails drop to 2
//   2: ramp counter down by 8 (clamp to 0 -> state 3); if test passes go to 1
//   3/default: idle; if test passes restart at 1
extern int func_001D2910(int arg, unsigned char state);
extern void func_001DF110(unsigned char *p);

void func_001DEEE0(unsigned char *p) {
    int a = *(int *)(p + 8);
    unsigned char state = *(unsigned char *)(p + 0);

    switch (state) {
    case 0:
        *(int *)(p + 0x1C) = 0;
        if (func_001D2910(a, state) == 0) {
            *(unsigned char *)(p + 0) = 3;
            return;
        }
        *(unsigned char *)(p + 0) = 1;
        return;
    case 1:
        if (func_001D2910(a, state) == 0) {
            *(unsigned char *)(p + 0) = 2;
        }
        *(int *)(p + 0x1C) = *(int *)(p + 0x1C) + 8;
        if (*(int *)(p + 0x1C) >= *(int *)(p + 4)) {
            *(int *)(p + 0x1C) = *(int *)(p + 4);
        }
        func_001DF110(p + 0x10);
        return;
    case 2:
        *(int *)(p + 0x1C) = *(int *)(p + 0x1C) - 8;
        if (*(int *)(p + 0x1C) <= 0) {
            *(int *)(p + 0x1C) = 0;
            *(unsigned char *)(p + 0) = 3;
        }
        if (func_001D2910(a, state) != 0) {
            *(unsigned char *)(p + 0) = 1;
        }
        func_001DF110(p + 0x10);
        return;
    case 3:
    default:
        if (func_001D2910(a, state) != 0) {
            *(unsigned char *)(p + 0) = 1;
        }
        return;
    }
}
