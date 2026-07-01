// NEARMISS func_0018CBD0  (vram 0x0018CBD0, 0x284 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc saved-float-register-coloring wall: target allocates $f21 for the speed->dist->ang call-spanning chain and a separate $f20 for the tail-local falloff blend; every C variant tried (decl order, dead-store shape, shared-vs-duplicated subexpression) reproduces byte-identical instructions except ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS objdiff 91.78% via mwcc233 (86.53% 991202). Logic/structure fully
// recovered: copies two source vec3 blocks (arg0+0x30 from D_70003B50, arg0+0x20
// from arg1+0xA0), rebuilds the actor's forward-facing identity+seed transform
// (func_001029C0/func_00102C58) applied to arg0+0x30, applies it via
// func_001026A0 to a scratch vec3 built from the caller's speed float
// (D_70003600 quad: 0/0/speed/1.0), adds the transformed offset into
// arg0+0x10..0x18, computes a horizontal distance via func_001028D0 + sqrtf
// (func_0011E748) and turns it into a speed-vs-heading delta `ang` via
// fabsf (func_0011DF78), then blends two 11.0f-based falloff curves into
// arg0+0x24/0x14 using per-branch constants selected by whether arg0+0x64
// is exactly -46.8f. Residual is the same mwcc saved-float-register-coloring
// wall proven elsewhere in this project: the target allocates $f21 for the
// speed->dist->ang call-spanning chain and a SEPARATE $f20 for the tail-local
// blend value; every C variant tried (decl order, dead-store shape, shared vs.
// duplicated subexpression) reproduces byte-identical instructions with the
// physical register numbers ($f20 vs $f21) swapped instead of matching. The
// mula.s/madd.s ACC pattern, load orders, and branch shapes ARE byte-identical.
extern void func_00102948(void *a0, void *a1);
extern void func_001029C0(void *a0);
extern void func_00102C58(void *a0, void *a1, float *a2);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028D0(void *a0, void *a1, void *a2);
extern float func_0011E748(float a0);
extern float func_0011DF78(float a0);
extern char D_70003400[];
extern char D_70003600[];
extern char D_70003B50[];

void func_0018CBD0(char *arg0, char *arg1, float fparg0) {
    float f20, f20b, t;
    float x, z, ang, f3, f4, f5;
    float speed;

    speed = fparg0;
    func_00102948(arg0 + 0x30, D_70003B50);
    func_00102948(arg0 + 0x20, arg1 + 0xA0);
    func_001029C0(D_70003400);
    func_00102C58(D_70003400, D_70003400, (float *)(arg0 + 0x30));
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(float *)0x70003608 = speed;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(arg0 + 0x10, D_70003400, D_70003600);
    *(float *)(arg0 + 0x10) = *(float *)(arg0 + 0x10) + *(float *)(arg0 + 0x20);
    *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x14) + *(float *)(arg0 + 0x24);
    *(float *)(arg0 + 0x18) = *(float *)(arg0 + 0x18) + *(float *)(arg0 + 0x28);
    func_001028D0(D_70003600, arg0 + 0x20, arg0 + 0x10);
    z = *(float *)0x70003600;
    x = *(float *)0x70003608;
    ang = func_0011E748(z * z + x * x);
    ang = ang - func_0011DF78(speed);

    if (*(float *)(arg0 + 0x64) == -46.79999923706055f) {
        f3 = -20.0f;
        f4 = 6.0f;
        f5 = 2.0f;
    } else {
        f3 = -10.0f;
        f4 = 2.0f;
        f5 = 6.0f;
    }

    /* shared f20: unconditionally computed, used by the field-0x24 store below */
    t = f3 - ang;
    f20 = t;
    if (ang < f3) {
        f20 = f3 + t;
        if (f20 <= -7.0f) {
            f20 = t;
        }
    }

    *(float *)(arg0 + 0x24) = 11.0f + (*(float *)(arg1 + 0xA4) + f4 + 0.30000001192092896f * f20);

    if (ang < f3) {
        f20b = ang - f3;
        if (f3 == -20.0f) {
            f20b = 0.5f * (ang - f3);
        } else if (f20b < -10.0f) {
            f20b = -10.0f;
        }
        *(float *)(arg0 + 0x14) = 11.0f + (f4 + (*(float *)(arg1 + 0xA4) + (f5 - f20b)));
    } else {
        *(float *)(arg0 + 0x14) = 11.0f + (f4 + (f5 + *(float *)(arg1 + 0xA4)));
    }
}
