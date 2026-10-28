// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 82.06% NEARMISS).
// Fixes read from the original instructions:
//  - D_0028A734 / D_0028A738 are absolute (unsized extern arrays), D_00275C70
//    gp-relative; callee prototypes taken from their definitions in src/
//    (func_00200780 takes pointers and returns int, func_00200830 a pointer);
//  - the header is read through the chained `desc = D_00275C70 = D_00289BC0`
//    value; the header buffer argument carries a (void *) cast, which sets
//    the original argument-setup order;
//  - each count is guarded by an explicit nonzero test around an unsigned
//    for-loop, and the relocation table pointer is computed inside the
//    guard; each sub-chunk's offset and size are read before its retry loop;
//  - the three loops use separate counters, and the chunk offset/size locals
//    are reused for the trailing copy (register assignment follows these
//    declarations).

// Streams a packed resource bank into memory. arg0 selects a base: 0 ->
// 0xB00000, else the running pointer in D_0028A734. It DMA-loads the 0x800-byte
// header (func_00200780/func_00200700 retry pair) into D_00289BC0 from offset
// arg0<<11, publishes it as the current bank D_00275C70, then for each of the
// header's [0xE] sub-chunks (n0, 8-byte entries from +0x20) reads entry
// [0x20]+header[4] (offset) and [0x24] (size) into the bank base and runs
// func_00200830 on it. After a trailing copy of header[8]-header[0x14] bytes
// from offset header[4]+header[0x14] it sets D_0028A734 and D_0028A738 to the
// end of that copy, walks the [0x10] table (cnt10) running func_00200830 over
// each stored chunk (chunk sizes read from the [n0+k] entry, field 0x24),
// then relocates the [0x1C] pointer table (cnt1c) starting at entry
// [n0+cnt10]: for each word e, D_0028A490[e>>24] = (e & 0xFFFFFF) + base.

extern int func_00200700(void);
extern int func_00200780(void *file, void *buf, int offset, int size);
extern void func_00200830(char *a);
extern char *D_00275C70;
extern char D_00289BC0[];
extern char D_0028A480[];
extern char D_0028A488[];
extern int D_0028A490[];
extern char *D_0028A734[];
extern char *D_0028A738[];
void func_001FF1E0(int arg0) {
    char *base;
    int off;
    int o;
    int sz;
    unsigned int n0;
    unsigned int i;
    unsigned int cnt10;
    unsigned int k;
    int j;
    char *desc;
    int *p;
    unsigned int cnt1c;
    unsigned int m;

    if (arg0 == 0) {
        base = (char *)0xB00000;
    } else {
        base = D_0028A734[0];
    }
    do {
        func_00200780(D_0028A480, (void *)D_00289BC0, arg0 << 11, 0x800);
    } while (func_00200700() == 0);

    desc = D_00275C70 = D_00289BC0;
    n0 = *(unsigned short *)(desc + 0xE);
    off = *(int *)(desc + 4);
    if (n0 != 0) {
        for (i = 0; i < n0; i++) {
            char *ent = D_00275C70 + i * 8;
            o = *(int *)(ent + 0x20) + off;
            sz = *(int *)(ent + 0x24);
            do {
                func_00200780(D_0028A488, base, o, sz);
            } while (func_00200700() == 0);
            func_00200830(base);
        }
    }

    sz = *(int *)(D_00275C70 + 0x14);
    o = *(int *)(D_00275C70 + 8) - sz;
    off += sz;
    do {
        func_00200780(D_0028A488, base, off, o);
    } while (func_00200700() == 0);
    D_0028A734[0] = base + o;
    D_0028A738[0] = base + o;

    cnt10 = *(unsigned int *)(D_00275C70 + 0x10);
    j = 0;
    if (cnt10 != 0) {
        for (k = 0; k < cnt10; k++) {
            func_00200830(base + j);
            j += *(int *)(D_00275C70 + (n0 + k) * 8 + 0x24);
        }
    }

    cnt1c = *(unsigned int *)(D_00275C70 + 0x1C);
    if (cnt1c != 0) {
        p = (int *)(D_00275C70 + (n0 + cnt10) * 8 + 0x20);
        for (m = 0; m < cnt1c; m++) {
            unsigned int e = *p++;
            D_0028A490[e >> 24] = (e & 0xFFFFFF) + (int)base;
        }
    }
}
