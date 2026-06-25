// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 2
// Actor bone/matrix-slot init: store bone count, bail (state=3) if it exceeds
// the global cap, else allocate one matrix per bone, run bone-array setup, set state=1.
extern void anim_bone_array_setup(unsigned char);
extern int func_001AF780(unsigned char);
extern short D_00275BCC;

int func_0022B700(char *arg0, unsigned char arg1) {
    int i;
    char *p;

    *(unsigned char *)(arg0 + 0xC) = arg1;
    if (D_00275BCC < (int)*(unsigned char *)(arg0 + 0xC)) {
        *(char *)(arg0 + 4) = 3;
        return 0;
    }
    i = 0;
    p = arg0;
    while (i < (int)*(unsigned char *)(arg0 + 0xC)) {
        *(int *)(p + 0x110) = func_001AF780(*(unsigned char *)(arg0 + 0xC));
        p += 4;
        i++;
    }
    anim_bone_array_setup(*(unsigned char *)(arg0 + 0xC));
    *(unsigned char *)(arg0 + 9) = *(unsigned char *)(arg0 + 0xC);
    *(char *)(arg0 + 4) = 1;
    return 1;
}
