// NEARMISS func_00117CB0  (vram 0x00117CB0, 0x64 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 70.00% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Body decoded correctly and compiles at 70%. Two residual diffs are genuine ee-gcc EE-codegen/scheduling decisions not steerable from C: (1) the SECOND of two adjacent integer divides is emitted by the target on the EE second integer pipe (div1/mflo1) while ee-gcc here emits a regular div/mflo for both — no C express...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned short D_0027F77A;

void func_00117CB0(unsigned char *a0) {
    int base = *(int *)(a0 + 8);
    unsigned char *p = *(unsigned char **)(a0 + 0xC) + base;
    int rate = *(unsigned short *)(a0 + 0x56);
    int val = (p[3] << 8) | p[2];
    *(unsigned short *)(a0 + 0x54) = val;
    *(int *)(a0 + 8) = base + 4;
    *(int *)(a0 + 0x1C) = ((rate * val) << 12) / (int)D_0027F77A / 0x3C;
}
