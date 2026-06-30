// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Per-frame state driver for the global object at *(0x70003B6C), dispatched on
// the state byte at offset +0xD (a 3-way switch 0/1/2).
//   state 0: kick func_001FBC50(0) + func_001FABB0(), then re-read the global
//            and bump the state byte (+0xD) by 1.
//   state 1: only while the gate D_00282157 is 0, bump the state byte, raise the
//            dirty flag D_00275BD8 = 1, and fire func_001FF080(0, 0x2B, &state).
//   state 2: return 1 once D_00275BD8 has been cleared (==0); otherwise 0.
// All paths fall through to return 0.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 89.7%. Keys: (1) func_001FBC50 takes a single (st) arg -- passing st (known 0
// in the case-0 arm) lets mwcc reuse a0 instead of re-zeroing it, dropping the
// extra paddub; (2) the case-1 store re-reads through the &state pointer
// (`*fld = *fld + 1`) and reuses the dispatch constant 1 as the D_00275BD8 store
// value; (3) ASCENDING case labels (0,1,2) reproduce the target's descending
// 2->1->0 compare chain; (4) D_00275BD8 is gp-rel (-sdatathreshold 8) while
// D_00282157 is forced non-gp-rel by declaring it as a >threshold array.
extern void func_001FABB0(void);
extern void func_001FBC50(unsigned char);
extern void func_001FF080(int, int, unsigned char *);
extern unsigned char D_00275BD8;
extern signed char D_00282157[9];

int func_0022A590(void) {
    unsigned char *p;
    unsigned char st;
    unsigned char *fld;

    p = *(unsigned char **)0x70003B6C;
    st = *(unsigned char *)(p + 0xD);
    fld = p + 0xD;
    switch (st) {
    case 0:
        func_001FBC50(st);
        func_001FABB0();
        p = *(unsigned char **)0x70003B6C;
        *(unsigned char *)(p + 0xD) = *(unsigned char *)(p + 0xD) + 1;
        break;
    case 1:
        if (D_00282157[0] == 0) {
            *fld = *fld + 1;
            D_00275BD8 = 1;
            func_001FF080(0, 0x2B, fld);
        }
        break;
    case 2:
        if (D_00275BD8 == 0) {
            return 1;
        }
        break;
    }
    return 0;
}
