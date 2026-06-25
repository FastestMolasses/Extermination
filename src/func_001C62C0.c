// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Like func_001C6200, but seeds each sub-object from a source table at
// *(arg0+0x44) (base + the int at +0xC). For each of arg0[0xC] entries: clears
// the render fields (short 0x64 = source short at +4, the 0x88/0x8A/0x8C shorts
// = 0x1000, 0x70..0x84 ints = 0) and copies four quadwords (src+0x10..0x40 ->
// dest+0..0x30). The source row stride is 0x50 bytes.
typedef unsigned __int128 uint128;

void func_001C62C0(char *arg0) {
    char *src;
    int i;
    char *t0;
    char *e;
    char *d;

    src = *(char **)(arg0 + 0x44);
    i = 0;
    e = arg0;
    t0 = src + *(int *)(src + 0xC);
    for (; i < *(unsigned char *)(arg0 + 0xC); i++) {
        *(short *)(*(char **)(e + 0x110) + 0x64) = *(short *)(t0 + 4);
        *(short *)(*(char **)(e + 0x110) + 0x88) = 0x1000;
        *(short *)(*(char **)(e + 0x110) + 0x8A) = 0x1000;
        *(short *)(*(char **)(e + 0x110) + 0x8C) = 0x1000;
        *(int *)(*(char **)(e + 0x110) + 0x7C) = 0;
        *(int *)(*(char **)(e + 0x110) + 0x80) = 0;
        *(int *)(*(char **)(e + 0x110) + 0x84) = 0;
        *(int *)(*(char **)(e + 0x110) + 0x70) = 0;
        *(int *)(*(char **)(e + 0x110) + 0x74) = 0;
        *(int *)(*(char **)(e + 0x110) + 0x78) = 0;
        d = *(char **)(e + 0x110);
        e += 4;
        *(uint128 *)(d + 0x00) = *(uint128 *)(t0 + 0x10);
        *(uint128 *)(d + 0x10) = *(uint128 *)(t0 + 0x20);
        *(uint128 *)(d + 0x20) = *(uint128 *)(t0 + 0x30);
        *(uint128 *)(d + 0x30) = *(uint128 *)(t0 + 0x40);
        t0 += 0x50;
    }
}
