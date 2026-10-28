// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 84.38% NEARMISS).
// Fixes read from the original instructions:
//  - the high-half shift is arithmetic: `(int)(v & 0xFFFF0000) >> 16`, so a
//    0xFFFF high half is id -1 ("no sound");
//  - the cached id D_00282160 is read once into a local for both tests, but the
//    sound-state words D_00282160..D_00282170 are volatile: after the three
//    0x1000 stores the original reloads every argument of func_001FB9F0 from
//    memory (including the id it just stored);
//  - the table walk is spelled as the byte-matched sibling func_001B0250 does
//    (area pointer array, room pointer, record += entry*0x30), and the id is
//    computed before the low half.
//
// Room sound select from the area spawn tables: indexes
// D_0024D650[area D_00810700][room D_00810701] to the room's spawn-record table,
// steps by entry D_00810702*0x30, and reads the packed word at record +0x20:
// high half (id, signed >>16) and low half (lo, &0xFFFF).
// When the current area D_00810700 == 0xB and event flag 0x30 (D_00810788) ==
// 0xFF, force id = 0x44E. If the cached id D_00282160 changed, release the old
// handle (func_0011A070(D_00282164) when the prior id != -1), store the new id,
// and for a valid (!= -1) id submit it via func_001FB9F0(id, 0x1000, 0x1000,
// 0x1000) (the raw sound submit, FINDINGS audio chain), caching the handle in
// D_00282164. Finally func_00119828(0, lo, lo) and func_00119828(1, lo, lo).
// -sdatathreshold 0 required (these globals are absolute).
extern void func_00119828(int, int, int);
extern void func_0011A070(int);
extern int func_001FB9F0(int, int, int, int);
extern int **D_0024D650[];
extern volatile int D_00282160;
extern volatile int D_00282164;
extern volatile int D_00282168;
extern volatile int D_0028216C;
extern volatile int D_00282170;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00810788;

void func_001FC280(void) {
    int v;
    int lo;
    int id;
    int old;
    int **pp;
    char *q;

    pp = D_0024D650[D_00810700];
    q = (char *)pp[D_00810701];
    q += D_00810702 * 0x30;
    v = *(int *)(q + 0x20);
    id = (int)(v & 0xFFFF0000) >> 16;
    lo = v & 0xFFFF;
    if (D_00810700 == 0xB) {
        if (D_00810788 == 0xFF) {
            id = 0x44E;
        }
    }
    old = D_00282160;
    if (old != id) {
        if (old != -1) {
            func_0011A070(D_00282164);
        }
        D_00282160 = id;
        if (id != -1) {
            D_00282170 = 0x1000;
            D_0028216C = 0x1000;
            D_00282168 = 0x1000;
            D_00282164 = func_001FB9F0(D_00282160, D_00282170, D_00282168, D_0028216C);
        }
    }
    func_00119828(0, lo, lo);
    func_00119828(1, lo, lo);
}
