// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Runs the active list of items for the current (D_00810700) / sub (D_00810701)
// index: looks up a 2D dispatch table D_0024D820[primary][secondary] to get a
// null-terminated array of item pointers, primes it via func_001B65C0, then
// walks the array calling func_001B6660 on each entry.

extern void func_001B65C0(int *list);
extern void func_001B6660(int item);

extern int D_0024D820[];
extern unsigned char D_00810700[8];
extern unsigned char D_00810701[8];

void func_001B6910(void) {
    int *p;

    p = (int *)D_0024D820[D_00810700[0]];
    if (p != 0) {
        int *q = (int *)((int *)p)[D_00810701[0]];
        func_001B65C0(q);
        do {
            int item = *q;
            q += 1;
            func_001B6660(item);
        } while (*q != 0);
    }
}
