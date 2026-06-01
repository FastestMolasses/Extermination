// CFLAGS: -O4,p -sdatathreshold 0
extern int D_00241050;

int dmac_channel_base(int a0) {
    int v1 = 0;
    if ((unsigned int)a0 < 0xaU) {
        int *base = &D_00241050;
        v1 = base[a0];
    }
    return v1;
}
