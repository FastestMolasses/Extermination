// NEARMISS func_00119EA0  (vram 0x00119EA0, 0x1CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 56.05% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc forward-branch-likely wall (confirmed s84). The inner slot-scan loop (.L00119FD8) requires three forward `bnel` annulled branches (the 0x2E/0x30/0x34 occupancy checks that skip to the $a0++ increment); ee-gcc 2.9-991111-01 emits branch-likely ONLY on loop back-edges, never forward. Also has broad arg-register/...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_0027E0C0[];
extern int D_0027C6C0[];
extern int D_00281AC0;
extern unsigned short D_0027F740[];

int func_00119EA0(int a0, int a1, int a2) {
    int idx = a0 & 0x7FFF;
    int s0 = -1;
    unsigned char *t0;
    int *e;
    int *hdr;
    unsigned char *base;
    int i;
    int x, y;
    unsigned short *p;
    int sel;

    if ((unsigned int)idx >= 0x80)
        return s0;
    if (a1 < 0 || (unsigned int)a1 >= 0x80)
        return s0;
    if (a2 < 0 || (unsigned int)a2 >= 0x80)
        return s0;

    t0 = &D_0027E0C0[0];
    e = (int *)((char *)D_0027C6C0 + idx * 12);
    hdr = (int *)e[1];
    base = (unsigned char *)e[0];

    *(int *)((char *)&D_00281AC0 + 0x10) = (int)hdr;
    *(int *)((char *)&D_00281AC0 + 0x28) = (int)((char *)hdr + hdr[7]);

    if ((int)base != 1)
        return s0;
    if (hdr[3] != 0x64685353)
        return s0;
    if (hdr[8] == -1)
        return s0;

    {
        unsigned char *blk = (unsigned char *)((char *)hdr + hdr[7]);
        if (*(unsigned short *)blk < a1)
            return s0;
        x = *(unsigned short *)(blk + (a1 << 1) + 2);
        if (x == 0xFFFF)
            return s0;
        sel = x >> 1;
        if (*(unsigned short *)(blk + ((x >> 1) << 1)) < a2)
            return s0;
        p = (unsigned short *)(blk + ((a2 + sel) << 1));
        y = *(unsigned short *)((char *)p + 2);
        blk = blk + y;

        i = 0;
        while (1) {
            if (*(unsigned short *)(t0 + 0x2E) != 0)
                goto next;
            if (*(int *)(t0 + 0x30) != 0)
                goto next;
            if (*(unsigned short *)(t0 + 0x34) != 0)
                goto next;
            *(unsigned short *)(t0 + 0x34) = 1;
            *(int *)(t0 + 0xC) = (int)blk;
            *(unsigned short *)(t0 + 0x24) = idx;
            *(unsigned short *)(t0 + 0x28) = a1;
            *(unsigned short *)(t0 + 0x26) = a2;
            *(unsigned short *)(t0 + 0x44) = 0x1000;
            *(int *)(t0 + 0x4C) = 0x1000;
            *(unsigned short *)(t0 + 0x54) = 0x3C;
            *(unsigned short *)(t0 + 0x32) = 1;
            *(int *)(t0 + 0x18) = i;
            *(int *)(t0 + 0x48) = 0x1000;
            *(int *)(t0 + 0x1C) = 0x1E0000 / D_0027F740[0x1D];
            if (((a0 & 0x8000) ? 0 : 1) == 1)
                *(unsigned short *)(t0 + 0x42) = 1;
            s0 = i;
            break;
next:
            i++;
            if (i >= 0x30)
                break;
            t0 += 0x78;
        }
    }
    return s0;
}
