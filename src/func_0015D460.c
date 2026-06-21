// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001AEDE0(int a, int b);

void func_0015D460(unsigned char *p) {
    unsigned char st;

    st = *(unsigned char *)(p + 5);
    switch (st) {
    case 0:
        *(unsigned char *)(p + 5) = (unsigned char)(st + 1);
        *(int *)(p + 0x220) = 0;
        *(char *)(p + 0) = 0;
        break;
    case 1:
        *(unsigned char *)(p + 5) = (unsigned char)(st + 1);
        func_001AEDE0(4, 0);
        break;
    }
}
