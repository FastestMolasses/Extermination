// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%) from the former NEARMISS body. The switch cases end in break, not return; a return made mwcc fill the compare-chain slots from the fall-through.

extern void func_00102948(void *, void *);
extern void func_001029C0(void *);
extern void func_00102B08(void *, void *, float);
extern void func_00102BB0(void *, void *, float);
extern void func_001031E0();
extern void func_001EFD90(int, void *, void *);
extern void func_001F0460(int, void *);
extern char D_700036A0[1];
extern char D_700036D0[1];
extern char D_700038A0[1];

void func_00187EE0(char *arg0) {
    struct {
        int f0;
        float f4;
        int f8;
        int fc;
    } sp20;
    unsigned char st;

    func_001031E0(&sp20);
    sp20.f4 -= 1.5f;
    st = *(unsigned char *)(arg0 + 0x23A);
    switch (st) {
    case 0:
        if (*(short *)(arg0 + 0x212) != 0) {
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
            func_00102B08(D_700036A0, D_700036A0, -*(float *)(arg0 + 0x9C));
            func_00102948(D_700036D0, &sp20);
            func_001F0460(1, D_700036A0);
            break;
        }
        func_001EFD90(0x80000011, &sp20, arg0 + 0xC0);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        break;
    case 5:
        func_001EFD90(0x80000028, &sp20, arg0 + 0xC0);
        break;
    case 0x5A:
        func_001031E0(D_700038A0, arg0 + 0xB0);
        *(volatile float *)0x700038A4 = *(float *)(arg0 + 0x250);
        func_001EFD90(0x80000065, D_700038A0, arg0 + 0xC0);
        break;
    case 8:
        func_001EFD90(0x80000066, &sp20, arg0 + 0xC0);
        break;
    case 0x5C:
        func_001031E0(D_700038A0, arg0 + 0xB0);
        *(volatile float *)0x700038A4 = *(float *)(arg0 + 0x250);
        func_001EFD90(0x80000067, D_700038A0, arg0 + 0xC0);
        break;
    case 6:
        func_001EFD90(0x80000005, &sp20, arg0 + 0xC0);
        break;
    case 0x5B:
        func_001031E0(D_700038A0, arg0 + 0xB0);
        *(volatile float *)0x700038A4 = *(float *)(arg0 + 0x250);
        func_001EFD90(0x8000001D, D_700038A0, arg0 + 0xC0);
        break;
    case 0xD:
    case 0xE:
        break;
    case 7:
        func_001EFD90(0x80000068, &sp20, arg0 + 0xC0);
        break;
    }
}
