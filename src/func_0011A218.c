// COMPILER: eegcc
// CFLAGS: -O2
// Table[a0] writer with bounds: entry stride 120; clamp a0<0x30, a1/a2 in [-0x1000,0x1000].
struct ent {
    char pad[0x78];
};
extern struct ent D_0027E0C0;

void func_0011A218(int a0, int a1, int a2) {
    char *p = (char *)&D_0027E0C0 + a0 * 0x78;
    if ((unsigned int)a0 < 0x30 &&
        a1 >= -0x1000 && a1 <= 0x1000 &&
        a2 >= -0x1000 && a2 <= 0x1000) {
        *(int *)(p + 0x48) = a1;
        *(int *)(p + 0x4C) = a2;
        *(short *)(p + 0x52) = 1;
    }
}
