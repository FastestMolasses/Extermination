// NEARMISS func_001D98A0  (vram 0x001D98A0, 0x638 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful.
// Remaining differences: the original keeps the shared 0.0f in callee-saved $f21 (one more FP
// save, frame 0x190) where mwcc 2.3.3 rematerializes the constant per use; the original sets up
// the cross-product and tint-matrix call arguments right to left (a2/a1 before a0), and leaves
// the switch compare-chain delay slots empty.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Matching wave 2026-10-27 (83.80% -> 95.25%): the camera vectors are 4-float arrays (so
// the 3-term length sum reloads them and fuses into the multiply-accumulate form, which
// was not a wall), the 4x4 helper matrices are separate locals in the original stack
// order (m at sp+0x40 with its translation row m[12..15], rot rows at sp+0x80..0xBF),
// -sdatathreshold 4 puts D_00275670 gp-relative (the arrays stay absolute), the 6.0
// eye distance is a local, scale[5] is written before scale[0], and the switch lists its
// cases in reverse of the original compare order (2.3.3 emits case compares in reverse
// source order). D_002531C0 is the constant up vector {0, 1, 0, 0}.
//
// Builds a camera/view orientation: writes a fixed forward vector D_00817F70 =
// {0,-1,0,0}, rotates it by an identity-based rotation, crosses D_00817FC0 rows with
// the up vector, forms side = (0, -fwd.z, fwd.y) normalized by 1/len via
// func_0011E748 (sqrt) into D_00817F80, up2 = fwd x side into D_00817F90, copies them
// into the D_00817FC0 matrix rows, builds the transposed rotation, a view translation
// -(arg0 - 6*fwd) in D_00817F60, then scale (fparg0/128) and bias (0.5) matrices, and
// multiplies a tint matrix (D_0026E550 / D_0026E590 / D_0026E5D0) chosen by the 16-bit
// code D_00810700:D_00810701 (or arg1), then copies the result to *(D_00275670+0x24B0).
extern void copy_qw4(void *, float *);
extern void func_001026A0(float *, float *, float *);
extern void func_001026D0(float *, float *, float *);
extern void func_00102718(float *, float *, float *);
extern void func_00102798(float *, float *);
extern void func_00102948(float *, float *);
extern void func_001029C0(float *);
extern void func_00102B08(float *, float *, float);
extern void func_00102BB0(float *, float *, float);
extern float func_0011E748(float);

typedef unsigned __int128 u128;
extern float D_002531C0[];
extern float D_0026E550[16];
extern float D_0026E590[16];
extern float D_0026E5D0[16];
extern int D_00275670;
extern unsigned char D_00810700[];
extern float D_00817F20[];
extern float D_00817F60[];
extern float D_00817F70[];
extern float D_00817F80[];
extern float D_00817F90[];
extern float D_00817FC0[];
extern float D_00817FD0[];
extern float D_00817FE0[];
extern float D_00817FF0[];

void func_001D98A0(float *arg0, int arg1, float fparg0) {
    float m[16];
    float rot[16];
    float rz[16];
    float up[4];
    float scale[16];
    float bias[16];
    float inv;
    float zero;
    int v;
    float dist;

    zero = 0.0f;
    D_00817F70[0] = 0.0f;
    D_00817F70[1] = -1.0f;
    D_00817F70[2] = 0.0f;
    D_00817F70[3] = 0.0f;
    func_001029C0(rz);
    func_00102B08(rz, rz, zero);
    func_00102BB0(rz, rz, zero);
    func_001026A0(D_00817F70, rz, D_00817F70);
    *(u128 *)up = *(u128 *)D_002531C0;
    func_00102718(D_00817FC0, D_00817FF0, up);
    func_00102718(D_00817FC0, up, D_00817FC0);
    D_00817F80[0] = zero;
    D_00817F80[1] = zero - D_00817F70[2];
    D_00817F80[2] = D_00817F70[1] - zero;
    D_00817F80[3] = 0.0f;
    inv = 1.0f / func_0011E748(D_00817F80[0] * D_00817F80[0] + D_00817F80[1] * D_00817F80[1] + D_00817F80[2] * D_00817F80[2]);
    D_00817F80[0] = D_00817F80[0] * inv;
    D_00817F80[1] = D_00817F80[1] * inv;
    D_00817F80[2] = D_00817F80[2] * inv;
    D_00817F90[0] = D_00817F70[1] * D_00817F80[2] - D_00817F70[2] * D_00817F80[1];
    D_00817F90[1] = D_00817F70[2] * D_00817F80[0] - D_00817F70[0] * D_00817F80[2];
    D_00817F90[2] = D_00817F70[0] * D_00817F80[1] - D_00817F70[1] * D_00817F80[0];
    D_00817F90[3] = 0.0f;
    func_00102948(D_00817FF0, D_00817F70);
    func_00102948(D_00817FE0, D_00817F80);
    func_00102948(D_00817FD0, D_00817F90);
    func_00102948(&rot[0], D_00817F80);
    func_00102948(&rot[4], D_00817F90);
    func_00102948(&rot[8], D_00817F70);
    rot[12] = 0.0f;
    rot[13] = 0.0f;
    rot[14] = 0.0f;
    rot[15] = 1.0f;
    func_00102798(rot, rot);
    dist = 6.0f;
    D_00817F60[0] = -(arg0[0] - D_00817F70[0] * dist);
    D_00817F60[1] = -(arg0[1] - D_00817F70[1] * dist);
    D_00817F60[2] = -(arg0[2] - D_00817F70[2] * dist);
    D_00817F60[3] = 1.0f;
    func_001029C0(m);
    func_00102948(&m[12], D_00817F60);
    func_001026D0(m, rot, m);
    copy_qw4(D_00817F20, m);
    func_001029C0(scale);
    scale[5] = 0.0078125f * fparg0;
    scale[0] = 0.0078125f * fparg0;
    scale[10] = 1.0f;
    scale[15] = 1.0f;
    func_001026D0(m, scale, m);
    func_001029C0(bias);
    bias[12] = 0.5f;
    bias[13] = 0.5f;
    bias[14] = 0.0f;
    bias[15] = 1.0f;
    func_001026D0(m, bias, m);
    if (arg1 != 0) {
        func_001026D0(m, D_0026E5D0, m);
    } else {
        v = (D_00810700[0] << 8) + D_00810700[1];
        switch (v) {
        case 0x1:
        case 0x200:
        case 0x201:
        case 0x300:
        case 0x301:
        case 0x302:
        case 0x400:
        case 0x500:
        case 0x600:
        case 0x700:
        case 0x701:
        case 0x702:
        case 0x703:
        case 0x704:
        case 0x801:
        case 0x802:
        case 0x803:
        case 0x804:
        case 0x805:
        case 0x806:
        case 0x900:
        case 0xA00:
        case 0xB00:
        case 0xC00:
        case 0xD00:
        case 0xE00:
        case 0xF00:
        case 0xF01:
        case 0x1000:
        case 0x1001:
        case 0x1100:
        case 0x1200:
        case 0x1300:
        case 0x1301:
        case 0x1400:
        case 0x1500:
            func_001026D0(m, D_0026E550, m);
            break;
        case 0x0:
        case 0x2:
        case 0x100:
        case 0x101:
        case 0x202:
        case 0x401:
        case 0x800:
        case 0x1600:
        case 0x601:
            func_001026D0(m, D_0026E590, m);
            break;
        }
    }
    copy_qw4((void *)(D_00275670 + 0x24B0), m);
}
