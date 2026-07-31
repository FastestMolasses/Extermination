// NEARMISS func_00119650  (vram 0x00119650, 0x13C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 79.62% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring + preheader address-materialization permutation (confirmed no-lever class). Logic fully recovered and verified: bounds check (unsigned id < 0x80), magic check hdr[0xC]==0x71735353, 12-byte-stride table gate D_0027C6C0[id]==1, then a 48-entry / 0x78-stride scan of D_0027E0C0 for...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_0027E0C0[];
extern unsigned char D_0027C6C0[];
extern unsigned char D_00281AC0[];
extern unsigned char D_0027F740[];

int func_00119650(int id, unsigned char *hdr) {
    unsigned char *e = D_0027E0C0;
    unsigned char *ctx = D_00281AC0;
    unsigned char *cfg = D_0027F740;
    int ret = -1;
    int i;
    unsigned char b;

    if ((unsigned int)id < 0x80 &&
        *(int *)(hdr + 0xC) == 0x71735353 &&
        *(int *)(D_0027C6C0 + id * 12) == 1) {
        for (i = 0; i < 0x30; i++, e += 0x78) {
            if (*(unsigned short *)(e + 0x2E) != 0) continue;
            if (*(int *)(e + 0x30) != 0) continue;
            if (*(unsigned short *)(e + 0x34) != 0) continue;

            *(unsigned char **)(ctx + 0x14) = hdr;
            ret = i;
            *(short *)(e + 0x2E) = 1;
            *(short *)(e + 0x24) = id;
            *(int *)(e + 0x8) = 0x110;
            *(unsigned char **)(e + 0xC) = hdr;
            *(int *)(e + 0x18) = i;

            b = hdr[0x110];
            e[0] = b;
            e[2] = hdr[0x111];
            e[1] = b;
            e[3] = hdr[0x112];
            *(int *)(e + 0x48) = 0x1000;
            *(int *)(e + 0x4C) = 0x1000;
            *(unsigned short *)(e + 0x56) = *(unsigned short *)(hdr + 2);
            *(unsigned short *)(e + 0x54) = ((unsigned short *)(hdr + 2))[1];
            *(int *)(e + 0x1C) = ((*(unsigned short *)(e + 0x56) *
                                   *(unsigned short *)(e + 0x54)) << 12) /
                                 *(unsigned short *)(cfg + 0x3A) / 0x3C;
            break;
        }
    }
    return ret;
}
