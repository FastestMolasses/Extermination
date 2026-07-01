// NEARMISS func_001C7C00  (vram 0x001C7C00, 0x2A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.85% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// see wall field above
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Interpolates a "keyframe" record (8 floats, stride 0x20 bytes: 7 data
// floats + 1 time float at +0x1C) from a table pointed to by arg1+0x10.
// fparg0 is a fractional frame time. Clamps to first/last keyframe, else
// interpolates linearly between the bracketing pair, tracking whether the
// interpolated "loop" flag (field +0x1C, abs'd) went negative (wrap).
extern int float_to_int(float);
extern int D_00275BFC;
extern char D_008106F3;

int func_001C7C00(float *arg0, float *arg1, float fparg0) {
    float *s1;
    float *a0;
    float *v1;
    float f4;
    float f1;
    float f20;
    float f0;
    int v0;

    s1 = (float *) ((char *) arg1 + 0x10);
    if (fparg0 < 0.0f) {
        arg0[0] = s1[0];
        v0 = 0;
        arg0[1] = s1[1];
        arg0[2] = s1[2];
        arg0[3] = s1[3];
        arg0[4] = s1[4];
        arg0[5] = s1[5];
        arg0[6] = s1[6];
        f0 = s1[7];
    } else if (!(fparg0 < *arg1)) {
        s1 = (float *) ((char *) s1 + ((float_to_int(*arg1) - 1) << 5));
        arg0[0] = s1[0];
        v0 = 0;
        arg0[1] = s1[1];
        arg0[2] = s1[2];
        arg0[3] = s1[3];
        arg0[4] = s1[4];
        arg0[5] = s1[5];
        arg0[6] = s1[6];
        f0 = s1[7];
    } else {
        f20 = fparg0 - (float) float_to_int(fparg0);
        v1 = (float *) ((char *) s1 + (float_to_int(fparg0) << 5));
        a0 = (float *) ((char *) v1 + 0x20);
        if (v1[7] < 0.0f) {
            f20 = 0.0f;
            D_008106F3 = 1;
            D_00275BFC = 0x20;
        } else {
            D_008106F3 = 0;
        }
        f4 = v1[7];
        if (f4 < 0.0f) {
            f4 = -f4;
        }
        f1 = a0[7];
        if (f1 < 0.0f) {
            f1 = -f1;
        }
        v0 = 1;
        f1 = f20 * (f1 - f4);
        arg0[0] = f20 * (a0[0] - v1[0]);
        arg0[1] = f20 * (a0[1] - v1[1]);
        f0 = f1 + f4;
        arg0[2] = f20 * (a0[2] - v1[2]);
        arg0[3] = f20 * (a0[3] - v1[3]);
        arg0[4] = f20 * (a0[4] - v1[4]);
        arg0[5] = f20 * (a0[5] - v1[5]);
        arg0[6] = 0.0f;
        arg0[0] = arg0[0] + v1[0];
        arg0[1] = v1[1] + arg0[1];
        arg0[2] = v1[2] + arg0[2];
        arg0[3] = v1[3] + arg0[3];
        arg0[4] = v1[4] + arg0[4];
        arg0[5] = v1[5] + arg0[5];
        arg0[6] = v1[6] + arg0[6];
        arg0[7] = f1;
    }
    arg0[7] = f0;
    return v0;
}
