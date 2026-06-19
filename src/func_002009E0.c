// CFLAGS: -O4,p -sdatathreshold 0
extern int DisableDmacHandler(int);
extern void func_00121A28(void *, int, int);
void func_002009E0(char *p, int off) {
    char *q = p;
    int n = *(int *)(q + 0x14);
    DisableDmacHandler(2);
    if (n != 0) { q += off; func_00121A28(q, 0, n); }
}
