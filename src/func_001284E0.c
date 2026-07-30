// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: spawns a type-2 particle/effect entity at position arg1 (direction arg3 when
// arg2 == 1), copies the owner's 0x9D/0x9E ids and a unit 0x60 colour, installs
// func_0012A5D0 as its think handler, and keeps the live count at 0x700031F4 below 11.
extern char *func_001AFA90(int);
extern void func_0012A5D0();

int func_001284E0(char *arg0, char *arg1, int arg2, char *arg3) {
    char *p;

    if (!(*(int *)0x700031F4 <= 0xA)) {
        return 0;
    }
    p = func_001AFA90(2);
    if (p != 0) {
        *(char *)(p + 3) = 0;
        *(char *)(p + 0xD) = (char)(arg2 + 0xB);
        *(char *)(p + 0x9A) = 0;
        *(float *)(p + 0xB0) = *(float *)(arg1 + 0);
        *(float *)(p + 0xB4) = *(float *)(arg1 + 4);
        *(float *)(p + 0xB8) = *(float *)(arg1 + 8);
        if (arg2 == 1) {
            *(float *)(p + 0xC0) = *(float *)(arg3 + 0);
            *(float *)(p + 0xC4) = *(float *)(arg3 + 4);
            *(float *)(p + 0xC8) = *(float *)(arg3 + 8);
        } else {
            *(int *)(p + 0xC0) = 0;
            *(int *)(p + 0xC4) = 0;
            *(int *)(p + 0xC8) = 0;
        }
        *(unsigned char *)(p + 0x9D) = *(unsigned char *)(arg0 + 0x9D);
        *(unsigned char *)(p + 0x9E) = *(unsigned char *)(arg0 + 0x9E);
        *(int *)(p + 0x60) = 0x3F800000;
        *(int *)(p + 0x64) = 0x3F800000;
        *(int *)(p + 0x68) = 0x3F800000;
        *(int *)(p + 0x6C) = 0x3F800000;
        *(int *)(p + 0x10) = (int)&func_0012A5D0;
        *(int *)0x700031F4 = *(int *)0x700031F4 + 1;
        return 1;
    }
    return 0;
}
