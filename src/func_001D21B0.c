typedef struct { int pad[2]; char *buf; } State;

extern State *D_00275670;

void func_001D21B0(int a0) {
    State *a1;
    char *buf;
    a1 = D_00275670;
    buf = a1->buf;
    buf[3] = 0x50;
    *(int *)(buf + 4) = a0;
    *(short *)buf = 0;
    a1 = D_00275670;
    a1->buf = a1->buf + 0x10;
}
