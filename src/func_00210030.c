// NEARMISS func_00210030  (vram 0x00210030, 0x188 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring permutation (target pins temps to volatile $f2/$f3 across whole function; mwcc uses $f8/$f9/$f10) plus else-block float-constant-pool load reschedule. Not the clean-store nop -> permuter class. Best 87.56% mwcc233 / 85.93% 991202.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Recomputes a target's 3x basis offset (fields +0xB0/+0xB4/+0xB8) from a tuned
// set of constants. arg1==0 path: derive a/b from the entity's own +0xA0/+0xA8
// scaled by +0x60. else path: derive a/b from the globals D_00810154/158/15C
// (t = D_00810154/2). Then seeds +0xB0..B8 with D_0081061C/2C/3C and accumulates
// three weighted basis terms: a per-frame term (D_00810618/28/38 * globalctx
// (D_00275670)+0x2468), then the b-weighted basis (D_00810610/20/30) and the
// a-weighted basis (D_00810614/24/34).
//
// NEARMISS: body/structure fully recovered. Residual at 87.6% (mwcc 2.3.3) is
// pure FP register coloring -- the target keeps the temporaries in the volatile
// $f2/$f3 (fv1/fv1f) pair across the whole function where mwcc spreads them
// over $f8/$f9/$f10 (ft0/ft1/ft2) -- plus an else-block float-constant-pool load
// reschedule (loads of D_00810158/15C deferred past the div). Not the clean-store
// nop; this is the FP-coloring permutation class -> permuter pass. The
// -sdatathreshold 8 + sized-array decls keep D_00275670 gp-rel while the
// 0x810xxx constants stay absolute (lui/%hi), matching the target's addressing.
extern int D_00275670;
extern float D_00810154[3];
extern float D_00810158[3];
extern float D_0081015C[3];
extern float D_00810610[3];
extern float D_00810614[3];
extern float D_00810618[3];
extern float D_0081061C[3];
extern float D_00810620[3];
extern float D_00810624[3];
extern float D_00810628[3];
extern float D_0081062C[3];
extern float D_00810630[3];
extern float D_00810634[3];
extern float D_00810638[3];
extern float D_0081063C[3];

void func_00210030(char *arg0, int arg1) {
    float a, b;
    float t;

    if (arg1 == 0) {
        a = (*(float *)(arg0 + 0xA0) + -85.0014f) * *(float *)(arg0 + 0x60);
        b = *(float *)(arg0 + 0xA8) * *(float *)(arg0 + 0x60);
    } else {
        t = D_00810154[0] / 2.0f;
        b = 0.0f + (D_00810158[0] * t);
        a = -60.0f + (D_0081015C[0] * t);
    }
    *(float *)(arg0 + 0xB0) = D_0081061C[0];
    *(float *)(arg0 + 0xB4) = D_0081062C[0];
    *(float *)(arg0 + 0xB8) = D_0081063C[0];
    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + (D_00810618[0] * *(float *)(D_00275670 + 0x2468));
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + (D_00810628[0] * *(float *)(D_00275670 + 0x2468));
    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + (D_00810638[0] * *(float *)(D_00275670 + 0x2468));
    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + (D_00810610[0] * b);
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + (D_00810620[0] * b);
    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + (D_00810630[0] * b);
    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + (D_00810614[0] * a);
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + (D_00810624[0] * a);
    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + (D_00810634[0] * a);
}
