// NEARMISS func_001D4FC0  (vram 0x001D4FC0, 0x1A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.12% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation. Body+control-flow byte-correct in shape (caching D_00275670 per-section + threshold-4 to keep the data symbols absolute lifted it from 55/59% to 72/81%). Residuals: s0<->s2 coloring swap (arg0 vs first-cursor), t0/t1 and a2/a3 coloring in the 2nd tag section...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern char *block_copy(char *, void *, int, char *);
extern int func_001CAAC0(float *, char *, int, char *);
extern int func_001D1F80(int, int, int, char *);
extern int vif_append_ref_tag(int, void *);
extern char D_00237450[256];
extern char D_002513D0[256];
extern char *D_00275670;
extern char D_00816A40[256];

void func_001D4FC0(char *arg0) {
    int sp4C;
    float sp48;
    float sp44;
    float sp40;
    int n;
    int chunk;
    char *first;
    char *run;
    char *blk;
    char *dl;
    char *tmp;

    dl = D_00275670;
    first = *(char **)(dl + 0x1C);
    run = arg0 + 0x40;
    *(char *)(first + 3) = 0x10;
    *(int *)(*(char **)(dl + 0x1C) + 4) = 0;
    *(short *)(*(char **)(dl + 0x1C) + 0) = 0xE;
    tmp = *(char **)(dl + 0x1C);
    *(char **)(dl + 0x1C) = tmp + 0xF0;
    block_copy(tmp + 0x10, D_002513D0, 0xE0, dl);
    vif_append_ref_tag(3, D_00237450);
    dl = D_00275670;
    *(char *)(*(char **)(dl + 0x1C) + 3) = 0x30;
    blk = D_00816A40 + (*(int *)(dl + 0x9C) << 7);
    *(char **)(*(char **)(dl + 0x1C) + 4) = blk;
    *(short *)(*(char **)(dl + 0x1C) + 0) = 8;
    *(char **)(dl + 0x1C) = *(char **)(dl + 0x1C) + 0x10;
    func_001D1F80(3, 2, 1, blk);
    n = *(int *)(arg0 + 0);
    if (n > 0) {
        do {
            chunk = 0x1F8;
            if (n < 0x1F8) {
                chunk = n;
            }
            n -= 0x1F8;
            dl = D_00275670;
            *(char *)(*(char **)(dl + 0x1C) + 3) = 0x30;
            *(char **)(*(char **)(dl + 0x1C) + 4) = run;
            run += chunk * 0x820;
            *(short *)(*(char **)(dl + 0x1C) + 0) = (short)(chunk * 0x82);
            *(char **)(dl + 0x1C) = *(char **)(dl + 0x1C) + 0x10;
        } while (n > 0);
    }
    dl = D_00275670;
    *(char *)(*(char **)(dl + 0x1C) + 3) = 0x60;
    *(int *)(*(char **)(dl + 0x1C) + 4) = 0;
    *(short *)(*(char **)(dl + 0x1C) + 0) = 0;
    *(char **)(dl + 0x1C) = *(char **)(dl + 0x1C) + 0x10;
    sp40 = *(float *)(arg0 + 0x34);
    sp44 = *(float *)(arg0 + 0x38);
    sp48 = *(float *)(arg0 + 0x3C);
    sp4C = 0x3F800000;
    func_001CAAC0(&sp40, first, 0x60, dl);
}
