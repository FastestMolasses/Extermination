// NEARMISS func_001172B8  (vram 0x001172B8, 0x170 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 61.66% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc forward-branch-likely wall (confirmed s84). The two table-scan loops are built entirely on forward branch-likely (beql/bnezl/bnel/beqzl annulling branches with the ctrp[0x30] reload in the annulled slot); our ee-gcc 2.9-991111-01 only emits branch-likely on loop back-edges, never forward conditionals, so clean...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_0027CCC0[];
extern int D_0027F740[];

int func_001172B8(int a0) {
    unsigned char *base = D_0027CCC0;
    int *ctrp = D_0027F740;
    int i;
    int idx;
    int best1 = -1;
    int best2 = -1;
    short min1 = -1;
    short min2 = -1;

    for (i = 0; i < 0x30; i++) {
        idx = ctrp[0x30] % 0x30;
        if (*(unsigned short *)(base + idx * 0x6A) != 0) {
            if (*(unsigned short *)(base + idx * 0x6A + 0x1A) == 3) {
                return idx;
            }
        }
        ctrp[0x30]++;
    }

    for (i = 0x2F; i >= 0; i--) {
        idx = ctrp[0x30] % 0x30;
        if (*(unsigned short *)(base + idx * 0x6A + 0x1A) == 1) {
            if (*(unsigned short *)(base + idx * 0x6A + 0x08) == 1) {
                if ((short)*(unsigned short *)(base + idx * 0x6A + 0x0A) < min1) {
                    best1 = idx;
                    min1 = *(unsigned short *)(base + idx * 0x6A + 0x0A);
                }
            } else {
                if ((short)*(unsigned short *)(base + idx * 0x6A + 0x0A) < min2) {
                    best2 = idx;
                    min2 = *(unsigned short *)(base + idx * 0x6A + 0x0A);
                }
            }
        }
        ctrp[0x30]++;
    }

    if (best1 != -1) {
        return best1;
    }
    if (best2 != -1) {
        return best2;
    }
    return -1;
}
