// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.00% NEARMISS).
// Fixes read from the original instructions:
// - The region base is an int pointer and its size word is read as one indexed
//   element (arg2[0x14002], byte offset 0x50008). The char-pointer spelling made
//   mwcc copy the base into a saved register at entry and split the offset.
// - The clamped destination capacity (*(arg1+0xC) - 4) is one local, kept live
//   across the region query and reused for the trailing (cap - len) argument.
// - The four out-values are separate locals declared a, c, b, d (stack slots
//   0x50/0x58/0x54/0x5C), and func_00203F40 stays unprototyped so its eight
//   arguments load in source order.
//
// Sets up a windowed block copy. The source cursor (arg1+8, +4) is wrapped
// into the region [arg2 .. arg2 + size word at byte 0x50008); the run length is
// clamped to the destination capacity (arg1+0xC, -4). Queries the active
// DMA/region descriptor via func_002063B0, performs the copy through
// func_00203F40, releases the region via func_00206470, and returns 1 if any
// bytes moved.
int func_00203F40();
void func_002063B0(char *a0, int *a1, int *a2, int *a3, int *t0);
void func_00206470(unsigned char *p, unsigned int n);
extern char D_002DF800[];

int func_00203E60(int unused, char *arg1, int *arg2) {
    int a, c, b, d;
    unsigned int src;
    unsigned int sz;
    unsigned int end;
    unsigned int cap;
    unsigned int n;
    int r;

    sz = arg2[0x14002];
    src = *(int *)(arg1 + 8) + 4;
    end = (unsigned int)arg2 + sz;
    if (src >= end) {
        src -= sz;
    }
    n = end - src;
    cap = *(int *)(arg1 + 0xC) - 4;
    if (cap < n) {
        n = cap;
    }
    func_002063B0(D_002DF800, &a, &b, &c, &d);
    r = func_00203F40(a, b, c, d, src, n, (unsigned int)arg2, cap - n);
    func_00206470((unsigned char *)D_002DF800, r);
    if (r > 0) {
        return 1;
    }
    return 0;
}
