// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf: scan the 0x30-entry D_0027E0C0 table, dispatch active/idle slots.
struct s0011A198 {
    char pad[0x2E];
    unsigned short f2E;
    unsigned short f30;
    unsigned short f32;
    char pad2[0x78 - 0x34];
};
extern struct s0011A198 D_0027E0C0;
extern void func_0011A070(int a0);

void func_0011A198(int a0) {
    struct s0011A198 *e = &D_0027E0C0;
    int hi = a0 << 15;
    int i = 0;
    do {
        if (e->f32 == 1) {
            if (e->f2E == 0) {
                func_0011A070(i | hi);
            }
        }
        i++;
        e++;
    } while (i < 0x30);
}
