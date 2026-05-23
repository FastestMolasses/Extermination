typedef struct { int pad[2]; char *buf; } State;

extern State *D_00275670;

void func_001D2180(int a0, short a1) {
    State *a2;
    char *buf;
    a2 = D_00275670;
    buf = a2->buf;
    buf[3] = 0x30;
    *(int *)(buf + 4) = a0;
    *(short *)buf = a1;
    a2 = D_00275670;
    a2->buf = a2->buf + 0x10;
}
