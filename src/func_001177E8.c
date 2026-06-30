// NEARMISS func_001177E8  (vram 0x001177E8, 0xD8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 56.35% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc forward-branch-likely wall (confirmed s84). Expected emits bnel/beqzl/bnezl on the non-loop weapon-table field-comparison chain (6 forward bnel skips + the beql/bnel store selector); our ee-gcc 2.9-991111-01 only emits branch-likely on loop back-edges, producing non-annulling bne/beqz/bnez plus extra explicit ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_0027CCC0[];
extern unsigned char D_0027F740[];

int func_001177E8(unsigned char *arg0) {
    int v18 = *(int *)(arg0 + 0x18);
    int v8 = *(int *)(arg0 + 0x8);
    unsigned char *e = D_0027CCC0 + 0x18;
    unsigned char *t = D_0027F740;
    long bit = 1;
    int i;

    for (i = 0x2F; i >= 0; i--, bit <<= 1) {
        unsigned short a3 = *(unsigned short *)(e - 0x18);
        if (a3 != 1) { e += 0x6A; continue; }
        if (*(unsigned short *)(e - 0x16) != *(unsigned char *)(arg0 + 2)) { e += 0x6A; continue; }
        if (*(unsigned short *)(e - 0x14) != (*(unsigned char *)(arg0 + 0) & 0xF)) { e += 0x6A; continue; }
        if (*(unsigned short *)(e - 0x12) != v18) { e += 0x6A; continue; }
        if (*(unsigned short *)(e + 2) != a3) { e += 0x6A; continue; }
        if (*(unsigned short *)(e + 0xA) != *(unsigned short *)(arg0 + 0x24)) { e += 0x6A; continue; }

        if (*(unsigned short *)(e - 0xC) == 0) {
            *(unsigned short *)(e - 0x10) = a3;
            *(unsigned short *)(e + 0) = 0;
        } else {
            if (*(unsigned short *)(e + 0) != 0) {
                *(unsigned short *)(e + 0) = 0;
            } else {
                *(unsigned short *)(e - 0x10) = a3;
            }
        }
        *(long *)(t + 0x28) |= bit;
        e += 0x6A;
    }

    *(int *)(arg0 + 0x8) = v8 + 3;
    return v18;
}
