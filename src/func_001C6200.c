// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Resets a run of sub-objects hung off arg0. For each of the arg0[0xC] entries,
// follows the pointer at slot 0x110 (an array of pointers indexed by the loop)
// and zeroes/clears its render fields: short 0x64 = -1, the three 0x88/0x8A/0x8C
// shorts = 0x1000, and the 0x70..0x84 ints = 0, then calls func_001029C0 on it.
void func_001029C0(int *p);

void func_001C6200(char *arg0) {
    int i;
    char *e;

    for (i = 0, e = arg0; i < *(unsigned char *)(arg0 + 0xC); e += 4, i++) {
        *(short *)(*(char **)(e + 0x110) + 0x64) = -1;
        *(short *)(*(char **)(e + 0x110) + 0x88) = 0x1000;
        *(short *)(*(char **)(e + 0x110) + 0x8A) = 0x1000;
        *(short *)(*(char **)(e + 0x110) + 0x8C) = 0x1000;
        *(int *)(*(char **)(e + 0x110) + 0x7C) = 0;
        *(int *)(*(char **)(e + 0x110) + 0x80) = 0;
        *(int *)(*(char **)(e + 0x110) + 0x84) = 0;
        *(int *)(*(char **)(e + 0x110) + 0x70) = 0;
        *(int *)(*(char **)(e + 0x110) + 0x74) = 0;
        *(int *)(*(char **)(e + 0x110) + 0x78) = 0;
        func_001029C0(*(int **)(e + 0x110));
    }
}
