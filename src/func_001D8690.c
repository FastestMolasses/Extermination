// NEARMISS func_001D8690  (vram 0x001D8690, 0x220 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body fully correct (gp-rel reload-per-access reproduced, sized-array non-gp 0x28 symbols). Sole residual (4 instrs of ~131): the clamp `if(c<=0)c=0;` emits plain `bc1f` instead of the target's `bc1tl` branch-likely with the first D_00275688 load duplicated into both branch slots + a `b` join. Bra...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Builds two blocks from the global state record at gp-rel D_00275688.
// arg2[0xC] is clamped: c = max(arg2[0xC] - 1.0f, 0.0f). The first block
// (arg0, 0x40 bytes) is a straight copy of the 3 rows at +0xC0/+0xD0/+0xE0/
// +0xF0 of D_00275688 with each row's 4th word zeroed. The second block
// (arg1) scales arg2[0..2] / clamped-c by the matrix at +0xF0..+0x128 and
// biases the last row by 8388608.0f.
//
// NEARMISS: readable C, body byte-correct, 97.6% on mwcc 2.3.3 (and 991202).
// Residual is a branch-likely (bc1tl) + delay-slot-fill scheduling artifact on
// the clamp, not the clean-store nop; permuter/scheduling class.

extern char *D_00275688;

void func_001D8690(char *arg0, char *arg1, char *arg2) {
    float c;

    c = *(float *)(arg2 + 0xC) - 1.0f;
    if (c <= 0.0f) {
        c = 0.0f;
    }
    *(float *)(arg0 + 0)    = *(float *)(D_00275688 + 0xC0);
    *(float *)(arg0 + 0x10) = *(float *)(D_00275688 + 0xC4);
    *(float *)(arg0 + 0x20) = *(float *)(D_00275688 + 0xC8);
    *(int *)(arg0 + 0x30) = 0;
    *(float *)(arg0 + 4)    = *(float *)(D_00275688 + 0xD0);
    *(float *)(arg0 + 0x14) = *(float *)(D_00275688 + 0xD4);
    *(float *)(arg0 + 0x24) = *(float *)(D_00275688 + 0xD8);
    *(int *)(arg0 + 0x34) = 0;
    *(float *)(arg0 + 8)    = *(float *)(D_00275688 + 0xE0);
    *(float *)(arg0 + 0x18) = *(float *)(D_00275688 + 0xE4);
    *(float *)(arg0 + 0x28) = *(float *)(D_00275688 + 0xE8);
    *(int *)(arg0 + 0x38) = 0;
    *(float *)(arg0 + 0xC)  = *(float *)(D_00275688 + 0xF0);
    *(float *)(arg0 + 0x1C) = *(float *)(D_00275688 + 0xF4);
    *(float *)(arg0 + 0x2C) = *(float *)(D_00275688 + 0xF8);
    *(int *)(arg0 + 0x3C) = 0;
    *(float *)(arg1 + 0)    = *(float *)(D_00275688 + 0xF0) * *(float *)(arg2 + 0);
    *(float *)(arg1 + 4)    = *(float *)(D_00275688 + 0xF4) * *(float *)(arg2 + 4);
    *(float *)(arg1 + 8)    = *(float *)(D_00275688 + 0xF8) * *(float *)(arg2 + 8);
    *(float *)(arg1 + 0xC)  = *(float *)(D_00275688 + 0xFC) * c;
    *(float *)(arg1 + 0x10) = *(float *)(D_00275688 + 0x100) * *(float *)(arg2 + 0);
    *(float *)(arg1 + 0x14) = *(float *)(D_00275688 + 0x104) * *(float *)(arg2 + 4);
    *(float *)(arg1 + 0x18) = *(float *)(D_00275688 + 0x108) * *(float *)(arg2 + 8);
    *(float *)(arg1 + 0x1C) = *(float *)(D_00275688 + 0x10C) * c;
    *(float *)(arg1 + 0x20) = *(float *)(D_00275688 + 0x110) * *(float *)(arg2 + 0);
    *(float *)(arg1 + 0x24) = *(float *)(D_00275688 + 0x114) * *(float *)(arg2 + 4);
    *(float *)(arg1 + 0x28) = *(float *)(D_00275688 + 0x118) * *(float *)(arg2 + 8);
    *(float *)(arg1 + 0x2C) = *(float *)(D_00275688 + 0x11C) * c;
    *(float *)(arg1 + 0x30) = 8388608.0f + *(float *)(D_00275688 + 0x120) * *(float *)(arg2 + 0);
    *(float *)(arg1 + 0x34) = 8388608.0f + *(float *)(D_00275688 + 0x124) * *(float *)(arg2 + 4);
    *(float *)(arg1 + 0x38) = 8388608.0f + *(float *)(D_00275688 + 0x128) * *(float *)(arg2 + 8);
    *(float *)(arg1 + 0x3C) = 8388608.0f + 64.0f * c;
}
