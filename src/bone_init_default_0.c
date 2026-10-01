// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001029C0(void *);

void bone_init_default_0(char *actor) {
    int i;
    char *p;

    i = 0;
    p = actor;
    while (i < *(unsigned char *)(actor + 0xC)) {
        *(short *)(*(char **)(p + 0x110) + 0x64) = -1;
        *(short *)(*(char **)(p + 0x110) + 0x88) = 0x1000;
        *(short *)(*(char **)(p + 0x110) + 0x8A) = 0x1000;
        *(short *)(*(char **)(p + 0x110) + 0x8C) = 0x1000;
        *(int *)(*(char **)(p + 0x110) + 0x7C) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x80) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x84) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x70) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x74) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x78) = 0;
        func_001029C0(*(char **)(p + 0x110));
        p += 4;
        i++;
    }
}
