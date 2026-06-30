// NEARMISS func_001D7100  (vram 0x001D7100, 0xA0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.88% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Pure register-allocation permutation: all instructions + schedule identical, only GPR names differ (target t0-t3 temps vs mwcc233 a4-a7/v0/v1). Not the clean-store nop; 2.3.3 already best build. Permuter pass should crack.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Sub-allocator: bumps a per-channel pointer in D_00275670[arg0].slot(+0x10),
// stamps a 0x20-byte header (count=n+1 at +0, byte 0x10 at base+3, +4=0), then
// fills a 0x10-byte command entry (q = p+0x10): {0, 0, 0x01000404,
// arg1|(n<<16)|0x6C000000} and block_copy's the payload to q+0x10. Returns the
// pre-bump base pointer. D_00275670 is gp-rel (sdatathreshold 8).
// NEARMISS: o233 98.88%. Every instruction and the full schedule match; sole
// residual is register coloring (target uses t0-t3 for temps, mwcc 2.3.3 uses
// a4-a7/v0/v1). Pure regalloc permutation -> permuter territory.
extern int D_00275670;
extern unsigned char *block_copy(unsigned char *dst, int src, int n, unsigned char *ctx);

unsigned char *func_001D7100(int arg0, int arg1, int arg2, int arg3) {
    int *slot = (int *)((arg0 * 4) + D_00275670);
    int n = arg3 >> 4;
    unsigned char *base = (unsigned char *)slot[4];
    unsigned char *p;
    unsigned char *q;

    base[3] = 0x10;
    *(int *)(((unsigned char *)slot[4]) + 4) = 0;
    *(short *)(((unsigned char *)slot[4])) = (short)(n + 1);
    p = (unsigned char *)slot[4];
    slot[4] = (int)(p + (n + 2) * 0x10);
    q = p + 0x10;
    *(int *)(q + 0) = 0;
    *(int *)(q + 4) = 0;
    *(int *)(q + 8) = 0x01000404;
    *(int *)(q + 0xC) = arg1 | (n << 0x10) | 0x6C000000;
    block_copy(q + 0x10, arg2, arg3, p);
    return base;
}
