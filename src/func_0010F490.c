// NEARMISS func_0010F490  (vram 0x0010F490, 0x90 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.69% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Branch-likely wall (oversized +4 instrs). Two identical byte-copy loops over a struct at base (a0|0x20000004): n0=off0, n4=off4, src0=off8, src1=off0xC, dest arrays at off0x10 and off0x50; loop1 copies n0 bytes guarded by (n0>0 && n4>0), loop2 copies n4 bytes guarded by (n4>0). Semantics correct. Expected folds the ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

struct s {
    int n0;            /* 0x00 */
    int n4;            /* 0x04 */
    unsigned char *src0; /* 0x08 */
    unsigned char *src1; /* 0x0C */
    unsigned char buf0[0x40]; /* 0x10 */
    unsigned char buf1[0x40]; /* 0x50 */
};

void func_0010F490(unsigned int a0) {
    struct s *p = (struct s *)(a0 | 0x20000004);
    int i;

    if (p->n0 > 0) {
        if (p->n4 > 0) {
            unsigned char *src = p->src0;
            for (i = 0; i < p->n0; i++) {
                p->buf0[i] = src[i];
            }
        }
    }
    if (p->n4 > 0) {
        if (p->n4 > 0) {
            unsigned char *src = p->src1;
            for (i = 0; i < p->n4; i++) {
                p->buf1[i] = src[i];
            }
        }
    }
}
