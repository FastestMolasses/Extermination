// NEARMISS func_00210030  (vram 0x00210030, 0x188 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.57% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8) (was 87.56%).
// Remaining differences: in the arg1 != 0 block only, the three float constants
// (-60, 0, 2) are materialised in reverse order and the two scaling multiplies
// swap places; no source order of the constants or statements moved them
// (constant-setup-order residual).
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
// The fixes that took it from 87.6%: the zero/-60 offsets of the arg1 != 0
// path are named locals (not folded literals), both branches step through
// their temporaries one operation at a time, and the per-frame term is
// accumulated with +=. The -sdatathreshold 8 + sized-array decls keep
// D_00275670 gp-rel while the 0x810xxx constants stay absolute.
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
        a = *(float *)(arg0 + 0xA0);
        a += -85.0014f;
        b = *(float *)(arg0 + 0xA8);
        a *= *(float *)(arg0 + 0x60);
        b *= *(float *)(arg0 + 0x60);
    } else {
        float bx = 0.0f;
        float ax = -60.0f;
        b = D_00810158[0];
        a = D_0081015C[0];
        t = D_00810154[0] / 2.0f;
        b *= t;
        a *= t;
        b = bx + b;
        a = ax + a;
    }
    *(float *)(arg0 + 0xB0) = D_0081061C[0];
    *(float *)(arg0 + 0xB4) = D_0081062C[0];
    *(float *)(arg0 + 0xB8) = D_0081063C[0];
    *(float *)(arg0 + 0xB0) += (D_00810618[0] * *(float *)(D_00275670 + 0x2468));
    *(float *)(arg0 + 0xB4) += (D_00810628[0] * *(float *)(D_00275670 + 0x2468));
    *(float *)(arg0 + 0xB8) += (D_00810638[0] * *(float *)(D_00275670 + 0x2468));
    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + (D_00810610[0] * b);
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + (D_00810620[0] * b);
    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + (D_00810630[0] * b);
    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + (D_00810614[0] * a);
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + (D_00810624[0] * a);
    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + (D_00810634[0] * a);
}
