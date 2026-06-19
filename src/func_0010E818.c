// COMPILER: eegcc
// CFLAGS: -O2
extern void func_0010C710(int a0);

#define W(p, off) (*(int *)((char *)(p) + (off)))

void func_0010E818(void *a0) {
    int a1 = W(a0, 0x34);
    int a2 = W(a1, 0x40);
    int t0, t1;
    if (W(a2, 0xC) == 0) {
        W(a2, 0xC) = a1;
    } else {
        W(W(a2, 0x10), 0x3C) = a1;
    }
    W(a2, 0x10) = a1;
    t0 = W(a0, 0x14);
    t1 = W(a0, 0x1C);
    W(a1, 0x20) = t0;
    W(a1, 0x1C) = t1;
    W(a1, 0x24) = W(a0, 0x20);
    W(a1, 0xC) = W(a0, 0x24);
    W(a1, 0x28) = W(a0, 0x28);
    W(a1, 0x2C) = W(a0, 0x2C);
    W(a1, 0x30) = W(a0, 0x30);
    W(a1, 0x34) = W(a0, 0x10);
    if (W(a2, 0x0) >= 0 && W(a2, 0x4) == 0) {
        func_0010C710(W(a2, 0x0));
    }
}
