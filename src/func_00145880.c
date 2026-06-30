// NEARMISS func_00145880  (vram 0x00145880, 0x114 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.57% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-register coloring (permuter-class): several reloaded float temps land in $f2/$ft0 vs target $f1; plus the &D_00810360 pointer goes to $a0 vs target $a1 (float-reserves-GPR-slot mismatch, not fixable by prototype). Not the clean-store nop.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern int func_001B1560(float, void *);
extern int D_00810360;

void func_00145880(char *arg0, char *arg1) {
    float v;
    int f;

    if (*(char *)(arg1 + 0x75) & 1) {
        return;
    }
    v = *(float *)(arg1 + 0x48) + -0.1f;
    *(float *)(arg1 + 0x48) = v;
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + v;
    f = *(char *)(arg1 + 0x75);
    if ((f & 2) && !(f & 0x80) && *(float *)(arg1 + 0x5C) <= 18.0f &&
        func_001B1560(1.1344640254974365f, &D_00810360) != 0) {
        *(unsigned short *)(arg0 + 0x52) = *(unsigned short *)(arg0 + 0x52) | 1;
        return;
    }
    *(float *)(arg1 + 0x4C) = *(float *)(arg1 + 0x50) + *(float *)(arg1 + 0x4C);
    *(float *)(arg0 + 0xB0) =
        *(float *)(arg0 + 0xB0) + *(float *)(arg1 + 0x4C) * func_0011E2A8(*(float *)(arg0 + 0xC4));
    *(float *)(arg0 + 0xB8) =
        *(float *)(arg0 + 0xB8) + *(float *)(arg1 + 0x4C) * func_0011DE90(*(float *)(arg0 + 0xC4));
}
