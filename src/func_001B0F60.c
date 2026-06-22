// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_001B0EA0(void *p);
extern int bone_init_default_2(void *p, short n);
extern int D_0028A574;

int func_001B0F60(void *p, int n) {
    if (func_001B0EA0(p) != 0) {
        return 1;
    }
    *(int *)((char *)p + 0x40) = D_0028A574;
    bone_init_default_2(p, (short)n);
    *(unsigned char *)((char *)p + 4) = *(unsigned char *)((char *)p + 4) + 1;
    return 0;
}
