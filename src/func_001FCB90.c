// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Tail-calls func_001FE070(base + base->ofs + entry[idx].ofs, a3, a0, a1) for
// the table header base = D_0028A498 (16-byte entries at +0x10).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern char *D_0028A498;
extern void func_001FE070(char *p, int a1, int a2, int a3);

typedef struct { int ofs; int w1, w2, w3; } Ent;
void func_001FCB90(int a0, int a1, int idx, int a3) {
    char *b = D_0028A498;
    Ent *e = (Ent *)(b + 0x10);
    func_001FE070(b + *(int *)b + e[idx].ofs, a3, a0, a1);
}
