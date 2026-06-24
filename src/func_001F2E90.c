// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
extern void anim_bone_array_setup(unsigned char);
extern void bone_init_default_0(char *);
extern int func_001AF780(unsigned char);
extern void func_001F2F90(char *, int, char *, unsigned char);
extern int *D_00275B40;
extern short D_00275BCC;

int func_001F2E90(char *arg0, char *arg1) {
    int i;
    char *p;
    *(unsigned char *)(arg0 + 0xC) = *(unsigned char *)(arg1 + 0x4C);
    if (D_00275BCC < (int)*(unsigned char *)(arg0 + 0xC)) {
        *(char *)(arg0 + 4) = 3;
        return 0;
    }
    i = 0;
    p = arg0;
    for (; i < (int)*(unsigned char *)(arg0 + 0xC); i++) {
        *(int *)(p + 0x110) = func_001AF780(*(unsigned char *)(arg0 + 0xC));
        p += 4;
    }
    anim_bone_array_setup(*(unsigned char *)(arg0 + 0xC));
    *(unsigned char *)(arg0 + 9) = *(unsigned char *)(arg0 + 0xC);
    bone_init_default_0(arg0);
    for (i = 0; i < (int)*(unsigned char *)(arg0 + 0xC); i++) {
        func_001F2F90(arg0 + 0xD0, D_00275B40[i], arg1, *(unsigned char *)(arg0 + 0xD));
    }
    *(char *)(arg0 + 4) = 1;
    return 1;
}
