// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf: probe a 16-byte-strided table in D_00281AC0 for key bounds.
struct s001178C0 {
    int unk0;
    unsigned char *cur;
};
extern struct s001178C0 D_00281AC0;

int func_001178C0(int a0, int idx, int key) {
    int r;
    if (a0 == 0xFF) {
        r = 1;
    } else {
        struct s001178C0 *s = &D_00281AC0;
        unsigned char *e;
        idx <<= 4;
        e = s->cur + idx;
        s->cur = e;
        r = 0;
        if (key >= e[0]) {
            int hi = e[1];
            r = (hi >= key);
        }
        s->cur = e - idx;
    }
    return r;
}
