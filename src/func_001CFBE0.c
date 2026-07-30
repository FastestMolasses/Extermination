// NEARMISS func_001CFBE0  (vram 0x001CFBE0, 0x400 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.96% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// EXPECTED-OBJECT CONSTRUCTION ISSUE, not a compiler wall - 2 of 256 instructions, both in the second dispatch (0xC0/0xC4). This function has TWO jump tables. mwcc emits each table into its OWN .rodata section (local objects @43 and @44, each 0x1C bytes, each at offset 0 of its section), so both di...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: Builds the GIF/DMA packet chain for one effect instance.
//   Free-space guard: the write cursor is *(int *)(D_00275670 + 0x18); the buffer end is
//   D_004F35C0 (D_00810E80 == 0, i.e. field/normal mode) or D_005635C0 (otherwise). If
//   fewer than 0x8000 bytes remain, nothing is emitted.
//   Selection: the owner's mode word *(int *)(st + 0x8C) picks one of two 7-entry
//   lookup tables indexed by `kind` (0..6) -- jtbl_0026E390 for mode 1, jtbl_0026E370
//   for modes 2/3/4 -- yielding a texture/state block `tbl` and a blend-parameter row
//   `idx` into D_00251260 (16-byte stride). Any other mode, or kind >= 7, leaves both
//   uninitialised (the original relies on the incoming s0/s1 values; reproduced here as
//   uninitialised locals).
//   Packet 1 (tag 7): GIFtag 0x6C050059, the four scalars xf+0x44/0x48/0x50/0x4C, the
//   4x4 quadwords at xf+0x00.., then a 0x60 quadword cleared and overwritten with
//   0x14000000 / 0x11000000.
//   Packet 2 (tag 9): when `copy` is set, nine quadwords are copied from `st` by hand
//   into the reserved space; otherwise func_001CB6B0 does the transfer.
//   Packet 3 (tag 1): GIFtag 0x6C090050 only.
//   Packet 4 (tag 0x10): GIFtag 0x6C0F006E + regs 0x01000101, then the 4x4 matrix at
//   D_70003A40, the 4x4 at *(xf + 0x40), the 4x4 at D_70003AC0, the quadword at
//   D_00275670+0xA0, two zero words, the scalar xf+0x54, a zero word, and finally the
//   selected D_00251260 row. func_001CB760 registers (tbl, row) and func_001CB900
//   closes the chain with the mode word.

typedef unsigned int u128 __attribute__((mode(TI)));

extern short D_00810E80[8];
extern char *D_00275670;

extern char D_004F35C0[];
extern char D_005635C0[];
extern char D_007635C0[];
extern char D_00230800[];
extern char D_00231770[];
extern char D_00232540[];
extern char D_00233800[];
extern char D_0023D930[];
extern char D_00251260[];
extern char D_70003A40[];
extern char D_70003AC0[];

extern char *func_001CB5F0(char *, int, int);
extern void func_001CB6B0(char *, int, int, char *);
extern void func_001CB760(char *, int, char *, char *);
extern void func_001CB900(char *, int, int);

void func_001CFBE0(int id, unsigned int kind, char *st, char *xf, int copy)
{
    int idx;
    char *tbl;
    char *r;
    char *d;
    char *m;
    char *ent;
    int n;
    int avail;
    u128 *g1;
    u128 *g2;

    if (D_00810E80[0] == 0) {
        avail = (int)D_004F35C0 - *(int *)(D_00275670 + 0x18);
    } else {
        avail = (int)D_005635C0 - *(int *)(D_00275670 + 0x18);
    }
    if (avail >= 0x8000) {
        switch (*(int *)(st + 0x8C)) {
        case 1:
            switch (kind) {
            case 0:
                idx = 0;
                tbl = D_00230800;
                break;
            case 1:
                idx = 2;
                tbl = D_00231770;
                break;
            case 6:
                idx = 2;
                tbl = D_0023D930;
                break;
            case 2:
                idx = 4;
                tbl = D_00232540;
                break;
            case 3:
                idx = 2;
                tbl = D_00233800;
                break;
            case 5:
                idx = 6;
                tbl = D_00231770;
                break;
            case 4:
                idx = 7;
                tbl = D_00230800;
                break;
            }
            break;
        case 2:
        case 3:
        case 4:
            switch (kind) {
            case 0:
                idx = 1;
                tbl = D_00230800;
                break;
            case 1:
                idx = 3;
                tbl = D_00231770;
                break;
            case 6:
                idx = 3;
                tbl = D_0023D930;
                break;
            case 2:
                idx = 5;
                tbl = D_00232540;
                break;
            case 3:
                idx = 3;
                tbl = D_00233800;
                break;
            case 5:
                idx = 6;
                tbl = D_00231770;
                break;
            case 4:
                idx = 7;
                tbl = D_00230800;
                break;
            }
            break;
        }

        r = func_001CB5F0(D_007635C0, id, 7);
        ((u128 *)r)[0] = 0;
        *(int *)(r + 0xC) = 0x6C050059;
        *(float *)(r + 0x10) = *(float *)(xf + 0x44);
        *(float *)(r + 0x14) = *(float *)(xf + 0x48);
        *(float *)(r + 0x18) = *(float *)(xf + 0x50);
        *(float *)(r + 0x1C) = *(float *)(xf + 0x4C);
        ((u128 *)r)[2] = ((u128 *)xf)[0];
        ((u128 *)r)[3] = ((u128 *)xf)[1];
        ((u128 *)r)[4] = ((u128 *)xf)[2];
        ((u128 *)r)[5] = ((u128 *)xf)[3];
        ((u128 *)r)[6] = 0;
        *(int *)(r + 0x60) = 0x14000000;
        *(int *)(r + 0x64) = 0x11000000;

        if (copy != 0) {
            d = func_001CB5F0(D_007635C0, id, 9);
            n = 9;
            m = st;
            do {
                n -= 1;
                *(u128 *)d = *(u128 *)m;
                d += 0x10;
                m += 0x10;
            } while (n != 0);
        } else {
            func_001CB6B0(D_007635C0, id, 9, st);
        }

        r = func_001CB5F0(D_007635C0, id, 1);
        ((u128 *)r)[0] = 0;
        *(int *)(r + 0xC) = 0x6C090050;

        r = func_001CB5F0(D_007635C0, id, 0x10);
        ((u128 *)r)[0] = 0;
        *(int *)(r + 8) = 0x01000101;
        *(int *)(r + 0xC) = 0x6C0F006E;
        g1 = (u128 *)D_70003A40;
        ((u128 *)r)[1] = g1[0];
        ((u128 *)r)[2] = g1[1];
        ((u128 *)r)[3] = g1[2];
        ((u128 *)r)[4] = g1[3];
        m = *(char **)(xf + 0x40);
        ((u128 *)r)[5] = ((u128 *)m)[0];
        ((u128 *)r)[6] = ((u128 *)m)[1];
        ((u128 *)r)[7] = ((u128 *)m)[2];
        ((u128 *)r)[8] = ((u128 *)m)[3];
        g2 = (u128 *)D_70003AC0;
        ((u128 *)r)[9] = g2[0];
        ((u128 *)r)[10] = g2[1];
        ((u128 *)r)[11] = g2[2];
        ((u128 *)r)[12] = g2[3];
        ((u128 *)r)[13] = ((u128 *)(D_00275670 + 0xA0))[0];
        *(int *)(r + 0xE0) = 0;
        *(int *)(r + 0xE4) = 0;
        *(float *)(r + 0xE8) = *(float *)(xf + 0x54);
        *(int *)(r + 0xEC) = 0;
        ent = D_00251260 + (idx * 0x10);
        ((u128 *)r)[15] = *(u128 *)ent;
        func_001CB760(D_007635C0, id, tbl, ent);
        func_001CB900(D_007635C0, id, *(int *)(st + 0x8C));
    }
}
