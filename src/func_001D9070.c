// NEARMISS func_001D9070  (vram 0x001D9070, 0x124 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.07% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Pure register-coloring permutation (FPR + GPR). After splitting the inner loop into two parallel pointers (load-base ld at +0x38, store-base st at +0x20, both advancing 0x40 -- matching the target's $t0/$t1 pair) every instruction matches in shape, order and scheduling (including the bc1fl branch...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int D_0028A56C;

void func_001D9070(void) {
    int *p;
    char *grp;
    char *st;
    char *ld;
    int i;
    int j;
    float v;
    float t;

    p = (int *)func_001C6120(D_0028A56C, 0x16);
    grp = (char *)p + 0x40;
    for (i = 0; i < *p; i += 1) {
        st = grp + 0x10;
        ld = st;
        for (j = 0; j < 0x20; j += 1) {
            t = *(float *)(ld + 0x38) / 190.0f;
            if (!(t <= 0.3f)) {
                v = 0.0f;
            } else if (t < 0.0f) {
                v = 1.0f;
            } else {
                v = 1.0f - t;
                if (v < 0.0f) {
                    v = 0.0f;
                }
                if (!(v <= 1.0f)) {
                    v = 1.0f;
                }
            }
            v = 2.0f * v;
            *(float *)(st + 0x2C) = v;
            *(float *)(st + 0x28) = v;
            *(float *)(st + 0x24) = v;
            *(float *)(st + 0x20) = v;
            ld += 0x40;
            st += 0x40;
        }
        grp += 0x820;
    }
}
