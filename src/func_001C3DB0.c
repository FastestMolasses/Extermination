// NEARMISS func_001C3DB0  (vram 0x001C3DB0, 0x2FC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.46% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// fp-register-coloring + list-scheduler-interleave: body/structure fully recovered (cross-product-magnitude + dot-product angle-between-vectors computation feeding a quaternion-style blend via func_001026D0). Residual is the mwcc list-scheduler interleaving the D_70003400 struct-copy loads/stores 1...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Computes a rotation/heading blend between arg0 and arg1 direction vectors:
// if the cached direction D_70003630 (from func_00102718(arg0,arg1)) is exactly
// zero, just copies arg2 into arg3 (no turn needed) and returns. Otherwise seeds
// a working basis (D_700038A0/D_70003600/D_70003630) via func_00102760/
// func_00102718, builds a 3x3-ish transform block D_70003400 (rows: prior basis,
// arg0, refreshed direction) and calls func_00102798 to derive D_70003440.
// Computes ang = atan2(|cross(arg0,arg1)|, dot(arg0,arg1)) using func_00102718
// (cross-product-style helper into D_700038B0) and func_0011E748/func_0011E620
// (sqrt + atan2-like). Builds a quaternion-ish block D_70003480 from cos/sin(ang)
// (func_0011DE90/func_0011E2A8), composes it onto D_70003400 via two
// func_001026D0 calls, transforms arg2 through it into arg3 via func_001026A0,
// then normalizes arg3 in place via func_00102760.

extern void func_00102718(void *a0, void *a1, void *a2);
extern void func_00102760(void *a0, void *a1);
extern void func_00102798(void *a0, void *a1);
extern void func_001029C0(void *a0);
extern void func_001026D0(void *a0, void *a1, void *a2);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern float func_0011DE90(float x);
extern float func_0011E2A8(float x);
extern float func_0011E620(float x, float y);
extern float func_0011E748(float x);

extern char D_70003400[];
extern char D_70003440[];
extern char D_700034C0[];
extern char D_70003600[];
extern char D_70003630[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_70003480[];

void func_001C3DB0(char *arg0, char *arg1, float *arg2, float *arg3) {
    float ang;

    func_00102718(D_70003630, arg0, arg1);

    if (*(float *)0x70003630 == 0.0f &&
        *(float *)0x70003634 == 0.0f &&
        *(float *)0x70003638 == 0.0f) {
        arg3[0] = arg2[0];
        arg3[1] = arg2[1];
        arg3[2] = arg2[2];
        return;
    }

    func_00102760(D_700038A0, D_70003630);
    func_00102718(D_70003600, D_700038A0, arg0);
    func_00102760(D_70003630, D_70003600);
    func_001029C0(D_70003400);

    *(float *)0x70003400 = *(float *)0x700038A0;
    *(float *)0x70003404 = *(float *)0x700038A4;
    *(float *)0x70003408 = *(float *)0x700038A8;
    *(float *)0x70003410 = *(float *)(arg0 + 0);
    *(float *)0x70003414 = *(float *)(arg0 + 4);
    *(float *)0x70003418 = *(float *)(arg0 + 8);
    *(float *)0x70003420 = *(float *)0x70003630;
    *(float *)0x70003424 = *(float *)0x70003634;
    *(float *)0x70003428 = *(float *)0x70003638;

    func_00102798(D_70003440, D_70003400);

    {
        float ay = *(float *)(arg0 + 4);
        float by = *(float *)(arg1 + 4);
        float ax = *(float *)(arg0 + 0);
        float bx = *(float *)(arg1 + 0);
        float az = *(float *)(arg0 + 8);
        float bz = *(float *)(arg1 + 8);
        float dot = ax * bx + ay * by + az * bz;

        func_00102718(D_700038B0, arg0, arg1);

        {
            float sx = *(float *)0x700038B0;
            float sy = *(float *)0x700038B4;
            float sz = *(float *)0x700038B8;
            float len2 = sx * sx + sy * sy + sz * sz;

            ang = func_0011E620(func_0011E748(len2), dot);
        }
    }

    func_001029C0(D_70003480);

    *(int *)0x70003480 = 0x3F800000;
    *(int *)0x70003490 = 0;
    *(int *)0x700034A0 = 0;
    *(int *)0x70003484 = 0;
    *(float *)0x70003494 = func_0011DE90(ang);
    *(float *)0x700034A4 = -func_0011E2A8(ang);
    *(int *)0x70003488 = 0;
    *(float *)0x70003498 = func_0011E2A8(ang);
    *(float *)0x700034A8 = func_0011DE90(ang);

    func_001026D0(D_700034C0, D_70003400, D_70003480);
    func_001026D0(D_70003480, D_700034C0, D_70003440);
    func_001026A0(arg3, D_70003480, arg2);
    func_00102760(arg3, arg3);
}
