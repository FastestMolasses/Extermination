// NEARMISS func_001D66A0  (vram 0x001D66A0, 0x28C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 67.06% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FPU-MAC wall (confirmed, dead class): the target computes the four rotation-seed floats using the PS2 FPU MAC-accumulator pipeline (mula.s f21,f26 / madd.s f23,f0,f2 and mula.s f22,f26 / msub.s f24,f0,f1). No plain-C arithmetic expression makes mwcc emit mula/madd/msub instead of separate mul.s+a...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Particle/spark emitter init: allocates 33 (0x21) particle records into the
// ring buffer at D_00275670[arg0]+0x10 (a per-slot particle-system context
// selected by arg0), each 0x40 bytes. Before the loop it writes one GS/GIF
// packet header into the record currently at the ring cursor (tag halfword
// 0x86, byte+3=0x10, word+4=0, then a fixed AD-header quadword
// 0x50000085/0x8021/0x40264000/0x4141) and advances the ring cursor by
// 0x870. Then it seeds a rotating 2D vector (f21,f22) = arg1[2..3] (a
// direction) rotated into fparg0's frame via sin/cos(fparg0), and a second
// pair (f23,f24) = the same vector rotated by (fparg0 + step) where
// step = 0.09817477 rad (~5.625 deg, i.e. 1/64 turn). Each of the 33
// iterations: copies arg2 (4 words) to +0x00 and arg3 (4 words) to +0x20 of
// the record; quantizes the base 2D point arg1[0..1] * 16 into +0x10/+0x14;
// quantizes a second point (base + 0.8*f21, base + 0.5*f22) * 16 into
// +0x30/+0x34; then advances the rotating pair one more `step` via the
// small-angle recurrence f21 -= k*f23; f22 -= k*f24; f23 += k*f21';
// f24 += k*f22'; with k = 2*sin(step) (so successive iterations trace out a
// spiral of emission points around the base position).
//
// NEARMISS -- NOT byte-matchable via plain C (FPU-MAC wall). The target
// computes the four seed values (f21/f22/f23/f24 before the loop) using the
// PS2 FPU's MAC-accumulator pipeline: `mula.s f21,f26` (ACC = f21*f26)
// followed by `madd.s f23,f0,f2` (f23 = f0*f2 + ACC), and similarly
// `mula.s f22,f26` / `msub.s f24,f0,f1` (f24 = f0*f1 - ACC). No plain-C
// multiply/add expression makes mwcc select the MAC pipeline for this
// pattern (confirmed instance of the documented FPU-MAC wall class,
// distinct from the idiom-13 clean-store case mwcc 2.3.3 fixes). A
// straight mul.s+add.s/sub.s translation (used below) is arithmetically
// equivalent but can never reproduce the target's opcode encoding, so
// objdiff sits well below 100% purely from the four missing MAC-form
// instructions plus their downstream scheduling; no source reshape
// recovers them. The per-iteration recurrence body (the loop, 0x21 reps)
// is a plain mul.s/add.s/sub.s chain in the target and IS reproduced
// faithfully below.
extern float func_0011E2A8(float x);
extern float func_0011DE90(float x);
extern int float_to_int(float x);
extern int D_00275670[];

char *func_001D66A0(int arg0, float *arg1, int *arg2, int *arg3, float fparg0) {
    float az, aw;
    float sinA, cosA, sinStep, cosStep;
    float f21, f22, f23, f24;
    float k;
    char *ctx;
    char *rec;
    char *cur;
    int i;

    az = arg1[2];
    aw = arg1[3];
    sinA = func_0011E2A8(fparg0);
    cosA = func_0011DE90(fparg0);
    sinStep = func_0011E2A8(0.09817477f);
    cosStep = func_0011DE90(0.09817477f);

    ctx = (char *)(D_00275670 + arg0 * 4);
    k = 2.0f * sinStep;

    rec = *(char **)(ctx + 0x10);
    rec[3] = 0x10;
    *(int *)(rec + 4) = 0;
    *(short *)(rec + 0) = 0x86;
    *(char **)(ctx + 0x10) = rec + 0x870;

    f21 = az * cosA;
    f23 = cosStep * (aw * sinA) + (az * cosA) * sinStep;  /* mula.s f21,f26 / madd.s f23,f0,f2 */
    f22 = az * sinA;
    f24 = cosStep * (aw * cosA) - (az * sinA) * sinStep;  /* mula.s f22,f26 / msub.s f24,f0,f1 */

    *(int *)(rec + 0x10) = 0;
    *(int *)(rec + 0x14) = 0;
    *(int *)(rec + 0x18) = 0;
    *(int *)(rec + 0x1C) = 0x50000085;
    *(int *)(rec + 0x20) = 0x8021;
    *(int *)(rec + 0x24) = 0x40264000;
    *(int *)(rec + 0x28) = 0x4141;
    *(int *)(rec + 0x2C) = 0;

    cur = rec + 0x30;
    i = 0;
    do {
        *(int *)(cur + 0x00) = arg2[0];
        *(int *)(cur + 0x04) = arg2[1];
        *(int *)(cur + 0x08) = arg2[2];
        *(int *)(cur + 0x0C) = arg2[3];
        *(int *)(cur + 0x20) = arg3[0];
        *(int *)(cur + 0x24) = arg3[1];
        *(int *)(cur + 0x28) = arg3[2];
        *(int *)(cur + 0x2C) = arg3[3];
        *(int *)(cur + 0x10) = float_to_int(16.0f * arg1[0]);
        *(int *)(cur + 0x14) = float_to_int(16.0f * arg1[1]);
        *(int *)(cur + 0x18) = 0xFFFFFF;
        *(int *)(cur + 0x1C) = 0;
        *(int *)(cur + 0x30) = float_to_int(16.0f * ((0.8f * f21) + arg1[0]));
        *(int *)(cur + 0x34) = float_to_int(16.0f * ((0.5f * f22) + arg1[1]));
        *(int *)(cur + 0x38) = 0xFFFFFF;
        *(int *)(cur + 0x3C) = 0;

        f21 -= k * f23;
        f22 -= k * f24;
        i += 1;
        cur += 0x40;
        f23 += k * f21;
        f24 += k * f22;
    } while (i < 0x21);

    return cur;
}
