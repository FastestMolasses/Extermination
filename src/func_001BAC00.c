// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Walks the 0x2C-byte entry list at *(arg2+0x14) until a -1 sentinel at +0.
// Entries tagged 0x270E at +4 spawn via func_001C8140 (dispatched through the
// D_0028A490 table indexed by the +6 short), and on success stamp r+0x24 from
// arg0+0x14 and set r+0x2E = 0xF. All other entries spawn via func_001AFA90
// keyed by the +0 byte; on success they copy bytes/floats from the entry into
// the new object, select the 0x10 handler (the entry's +0x28 pointer when the
// +0xA short != 3 and that pointer is non-null, else func_001BB0E0), record
// the entry pointer and arg0+0x14, stamp the running index, and bump it.
// Finally clears arg0+0x2E and returns 1.
//
// Matched with mwcc 2.3.3; the pinned 991202 build reaches 98.7% (a stray nop
// in the index-bump path). Notes: the live function takes three params (the
// list lives off the third, arg2); the index counter is a plain int (a short
// would force a redundant sign-extend the target does not emit); and the
// 0x10-handler select is written as two explicit stores (one per arm) to match
// the target's branch sense. objdiff 100.0 vs expected.
extern void *func_001AFA90(unsigned char a);
extern void *func_001C8140(int a, short b, void *c);
extern int D_0028A490[];
extern void func_001BB0E0(void);

int func_001BAC00(char *arg0, char *arg1, char *arg2) {
    int idx;
    char *e;
    char *r;

    e = *(char **)(arg2 + 0x14);
    idx = 0;
    do {
        if (*(short *)(e + 4) == 0x270E) {
            r = func_001C8140(D_0028A490[*(short *)(e + 6)], *(short *)(e + 8), *(void **)(e + 0x28));
            if (r != 0) {
                *(int *)(r + 0x24) = *(int *)(arg0 + 0x14);
                *(short *)(r + 0x2E) = 0xF;
            }
        } else {
            r = func_001AFA90(*(unsigned char *)(e + 0));
            if (r != 0) {
                void *fp;
                *(unsigned char *)(r + 3) = *(unsigned char *)(e + 2);
                *(unsigned char *)(r + 0xD) = (unsigned char)*(short *)(e + 4);
                *(float *)(r + 0xB0) = *(float *)(e + 0x10);
                *(float *)(r + 0xB4) = *(float *)(e + 0x14);
                *(float *)(r + 0xB8) = *(float *)(e + 0x18);
                *(float *)(r + 0xC0) = *(float *)(e + 0x1C);
                *(float *)(r + 0xC4) = *(float *)(e + 0x20);
                *(float *)(r + 0xC8) = *(float *)(e + 0x24);
                if (*(short *)(e + 0xA) != 3 && (fp = *(void **)(e + 0x28)) != 0) {
                    *(void **)(r + 0x10) = fp;
                } else {
                    *(void **)(r + 0x10) = (void *)func_001BB0E0;
                }
                *(char **)(r + 0x20) = e;
                *(int *)(r + 0x24) = *(int *)(arg0 + 0x14);
                *(short *)(r + 0x2E) = idx;
            }
            idx += 1;
        }
        e += 0x2C;
    } while (*(short *)e != -1);
    *(short *)(arg0 + 0x2E) = 0;
    return 1;
}
