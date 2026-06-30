// NEARMISS func_001D98A0  (vram 0x001D98A0, 0x638 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.80% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FPU-MAC (3-term dot-product no madd fusion) + shared-0.0f FP constant coloring in callee-saved $f21 (frame 0x190 vs 0x180 -> stack-offset cascade) + D_00275670 gp-rel/sdata mismatch + switch beq-chain order. Body fully correct; 2-term cross-product MAC matched. 2.3.3 does not fix; permuter/codege...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 83.8% (mwcc 2.3.3; 991202 caps 69.8%). Body/logic FULLY recovered
// and verified against the disasm. Builds a camera/view orientation: writes a
// fixed forward vector (D_00817F70..78 = {0,-1,0}), runs the matrix helpers,
// normalizes the up vector (D_00817F80..88) by 1/len via func_0011E748 (sqrt),
// forms the right vector as the cross product (F74*F88-F78*F84, etc.), assembles
// model/scale/bias matrices, and selects a tint matrix (D_0026E550 / D_0026E590
// / D_0026E5D0) by a 16-bit code built from D_00810700:D_00810701, then copies
// the result to *(D_00275670+0x24B0).
//
// RESIDUAL WALLS (not the clean-store nop; mwcc 2.3.3 does NOT fix):
//  (a) FPU-MAC: the 3-term length dot-product F80^2+F84^2+F88^2 -> CW emits
//      `mul;mul;adda.s;madd.s`; mwcc emits `mul;mul;mul;add;add` (no MAC fusion
//      for a 3-term scalar sum). The 2-term cross-product MAC (mula/msub) DOES
//      match. The 3-term sum is the documented FPU-MAC wall.
//  (b) FP constant coloring: CW keeps the shared 0.0f in callee-saved $f21
//      (extra swc1/lwc1, frame 0x190); mwcc rematerializes `mtc1 zero` per use
//      (frame 0x180). This shifts all stack offsets -> dominates the % delta.
//  (c) D_00275670 is gp-relative (sdata) in CW; mwcc emits lui/%lo at thr 0
//      (thr 4/8 fixes this one but breaks the bulk -> net worse).
//  (d) switch beq-chain dispatch order differs (CW vs mwcc lowering).
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
extern u128 D_002531C0;
extern float D_0026E550;
extern float D_0026E590;
extern float D_0026E5D0;
extern int D_00275670;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern float D_00817F20;
extern float D_00817F60;
extern float D_00817F64;
extern float D_00817F68;
extern int D_00817F6C;
extern float D_00817F70;
extern float D_00817F74;
extern float D_00817F78;
extern int D_00817F7C;
extern float D_00817F80;
extern float D_00817F84;
extern float D_00817F88;
extern int D_00817F8C;
extern float D_00817F90;
extern float D_00817F94;
extern float D_00817F98;
extern int D_00817F9C;
extern float D_00817FC0;
extern float D_00817FD0;
extern float D_00817FE0;
extern float D_00817FF0;

void func_001D98A0(float *arg0, int arg1, float fparg0) {
    float spC0[16];
    float sp110[16];
    float sp150[16];
    float sp40[12];
    float sp70[4];
    float sp80[4];
    float sp90[4];
    float spA0[4];
    float spB0[4];
    float sp100[4];
    float inv;
    float zero;
    int v;

    zero = 0.0f;
    D_00817F70 = 0.0f;
    D_00817F74 = -1.0f;
    D_00817F78 = 0.0f;
    D_00817F7C = 0;
    func_001029C0(spC0);
    func_00102B08(spC0, spC0, zero);
    func_00102BB0(spC0, spC0, zero);
    func_001026A0(&D_00817F70, spC0, &D_00817F70);
    *(u128 *)sp100 = D_002531C0;
    func_00102718(&D_00817FC0, &D_00817FF0, sp100);
    func_00102718(&D_00817FC0, sp100, &D_00817FC0);
    D_00817F80 = zero;
    D_00817F84 = zero - D_00817F78;
    D_00817F88 = D_00817F74 - zero;
    D_00817F8C = 0;
    inv = 1.0f / func_0011E748(D_00817F80 * D_00817F80 + D_00817F84 * D_00817F84 + D_00817F88 * D_00817F88);
    D_00817F80 = D_00817F80 * inv;
    D_00817F84 = D_00817F84 * inv;
    D_00817F88 = D_00817F88 * inv;
    D_00817F90 = D_00817F74 * D_00817F88 - D_00817F78 * D_00817F84;
    D_00817F94 = D_00817F78 * D_00817F80 - D_00817F70 * D_00817F88;
    D_00817F98 = D_00817F70 * D_00817F84 - D_00817F74 * D_00817F80;
    D_00817F9C = 0;
    func_00102948(&D_00817FF0, &D_00817F70);
    func_00102948(&D_00817FE0, &D_00817F80);
    func_00102948(&D_00817FD0, &D_00817F90);
    func_00102948(sp80, &D_00817F80);
    func_00102948(sp90, &D_00817F90);
    func_00102948(spA0, &D_00817F70);
    spB0[0] = 0.0f;
    spB0[1] = 0.0f;
    spB0[2] = 0.0f;
    spB0[3] = 1.0f;
    func_00102798(sp80, sp80);
    D_00817F60 = -(arg0[0] - D_00817F70 * 6.0f);
    D_00817F64 = -(arg0[1] - D_00817F74 * 6.0f);
    D_00817F68 = -(arg0[2] - D_00817F78 * 6.0f);
    D_00817F6C = 0x3F800000;
    func_001029C0(sp40);
    func_00102948(sp70, &D_00817F60);
    func_001026D0(sp40, sp80, sp40);
    copy_qw4(&D_00817F20, sp40);
    func_001029C0(sp110);
    sp110[0] = 0.0078125f * fparg0;
    sp110[5] = 0.0078125f * fparg0;
    sp110[10] = 1.0f;
    sp110[15] = 1.0f;
    func_001026D0(sp40, sp110, sp40);
    func_001029C0(sp150);
    sp150[12] = 0.5f;
    sp150[13] = 0.5f;
    sp150[14] = 0.0f;
    sp150[15] = 1.0f;
    func_001026D0(sp40, sp150, sp40);
    if (arg1 != 0) {
        func_001026D0(sp40, &D_0026E5D0, sp40);
    } else {
        v = (D_00810700 << 8) + D_00810701;
        switch (v) {
        case 0x601:
        case 0x1600:
        case 0x800:
        case 0x401:
        case 0x202:
        case 0x101:
        case 0x100:
        case 0x2:
        case 0x0:
            func_001026D0(sp40, &D_0026E590, sp40);
            break;
        case 0x1500:
        case 0x1400:
        case 0x1301:
        case 0x1300:
        case 0x1200:
        case 0x1100:
        case 0x1001:
        case 0x1000:
        case 0xF01:
        case 0xF00:
        case 0xE00:
        case 0xD00:
        case 0xC00:
        case 0xB00:
        case 0xA00:
        case 0x900:
        case 0x806:
        case 0x805:
        case 0x804:
        case 0x803:
        case 0x802:
        case 0x801:
        case 0x704:
        case 0x703:
        case 0x702:
        case 0x701:
        case 0x700:
        case 0x600:
        case 0x500:
        case 0x400:
        case 0x302:
        case 0x301:
        case 0x300:
        case 0x201:
        case 0x200:
        case 0x1:
            func_001026D0(sp40, &D_0026E550, sp40);
            break;
        }
    }
    copy_qw4((void *)(D_00275670 + 0x24B0), sp40);
}
