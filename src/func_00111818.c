// NEARMISS func_00111818  (vram 0x00111818, 0xA0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.75% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc delay-slot-fill placement wall. Logic/structure fully recovered: uncached-segment alias (arg|0x20000000), two guarded byte-copy loops (count reloaded each iteration from p[0] / p[4], separate hoisted source base p+0x10 / p+0x50 — matches expected's 'addu v0,a3,a1' + 'lbu 0(v0)' exactly), th...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241D48;
extern void func_001115D0(int *p);

void func_00111818(void *arg) {
    unsigned char *p = (unsigned char *)((unsigned int)arg | 0x20000000);
    unsigned char *dst;
    unsigned char *src;
    int i;

    if (*(int *)p > 0) {
        dst = *(unsigned char **)(p + 8);
        src = p + 0x10;
        for (i = 0; i < *(int *)p; i++) {
            dst[i] = src[i];
        }
    }
    if (*(int *)(p + 4) > 0) {
        dst = *(unsigned char **)(p + 0xC);
        src = p + 0x50;
        for (i = 0; i < *(int *)(p + 4); i++) {
            dst[i] = src[i];
        }
    }
    func_001115D0(&D_00241D48);
}
