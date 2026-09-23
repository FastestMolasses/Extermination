// NEARMISS func_00187780  (vram 0x00187780, 0x53C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.79% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is register coloring: arg2 lands in $s5 (target $s0) which shifts every saved GPR by one, and the 17.0/3.5/cone-length floats land in $f21/$f22/$f23 (target $f22/$f23/$f21), plus a few scheduling slots in the view-angle attenuation. Permuter-class.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Gun-light (flashlight) cone draw. Sole caller func_00188ED0, which calls it
// only while D_008106C7 (the gun-light enable) is set AND the player state
// pair D_008102B4/B5 is (1, 0x1D/0x1E..0x20) or (2, 0x17/0x18); outside that
// set 00188ED0 clears D_008106C7 instead (0x00189070). Calls use
// arg1 = (func_001B0070() & 0x80) ? 0 : 1 and arg2 = mode from D_008104A0.
// Dispatches on (arg2, arg1) to seed the size (64/32), the 17.0/3.5 floats, the
// spB0 base vector (16 or 8 in xyz), the spC0 random vector (5+3*rand or 2+rand
// in xyz via func_00122BB8), and a packed 64-bit word (0x20045D05554221F6 for
// arg2==0, 0x20048D0599422050 for arg2==1) passed to func_00187690 in $t0.
//
// Light matrix spE0 (one 4x4 at sp+0xE0; rows at +0x10/+0x20/+0x30): identity,
// rows 0..2 copied from *D_00275B40 +0x90/+0xA0/+0xB0, times rotY(pi/2)
// (func_00102BB0/func_001026D0), translated by arg0+0xB0 (func_00102918).
// A point 250 units along its local Z (func_001026A0) is ray-tested from the
// matrix origin row with func_0019A570(origin, end, 6, 0) (mask bit1 static
// world, bit2 heightfield). On hit: sp90 = hit point (0x700031B0) - origin, the
// length via the FPU multiply-accumulate idiom and func_0011E748, cone length
// f21 = min(len, 250), scale = min(len, 17)/17, spD0 = hit-record normal
// (*0x700031D0 + 0x24). On miss: f21 = 250, scale 1.0, spD0 = -(matrix row 2).
// The size and both floats are scaled by that factor. arg2==0 additionally
// attenuates spB0 when |dot(normalize(origin - D_008105D0), normalize(row 2))|
// exceeds 0.7 (func_00103230 by 1 - 0.8*(d-0.7)/0.3).
// Then: func_0021B9A0(2, 0, -140), func_0021B9A0(3, 0, 450.5), func_00187690
// (light matrix, spB0, spC0, (int)size, word, 17-scaled, 0.1, 3.5-scaled);
// unless area flag func_001B0070() & 0x20000000, func_001D9530(light matrix)
// draws the cone shells; if arg2==1 a second func_00187690 pass with spB0*2,
// spC0*1, word 0x20045D05554221F6 and 0.75x the 3.5-scaled float; finally
// func_0021B9A0(1, 0, 0).
//
// s87 corrections vs the earlier 78.62% version (all from the .s): spE0 was
// declared as four separate 16-byte arrays although the helpers treat it as
// one 4x4 matrix (the old "stack frame too small" wall); the $s1 "dead 64-bit
// constant" is func_00187690's $t0 argument; the matrix rows come from
// *D_00275B40 (a missing dereference; D_00275B40 is gp-relative, hence
// -sdatathreshold 4); the hit branch subtracts the origin from the 0x700031B0
// hit point (not D_008105D0), and the arg2==0 branch subtracts D_008105D0
// from the matrix origin (not from arg0). Measured: the variant calling
// func_001D9530(light_mtx) with one argument scores 86.24% because the caller
// really does load a1..a3/$f12; -sdatathreshold 0 scores 89.80%.

extern int float_to_int(float);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001026D0(void *a0, void *a1, void *a2);
extern float func_00102738(void *a0, void *a1);
extern void func_00102760(void *a0, void *a1);
extern void func_001028D0(void *a0, void *a1, void *a2);
extern void func_00102900(void *a0, void *a1, float a2);
extern void func_00102918(void *a0, void *a1, void *a2);
extern void func_00102948(void *a0, void *a1);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float a2);
extern void func_00103230(void *a0, void *a1, float a2);
extern float func_0011DF78(float a0);
extern float func_0011E748(float a0);
extern int func_00122BB8(void);
extern void func_00187690(void *a0, void *a1, void *a2, int a3, long long a4, float f12, float f13, float f14); /* byte-matched: arg4 in $t0 */
extern int func_0019A570(void *a0, void *a1, int a2, int a3);
extern int func_001B0070(void);
/* 0x00187BD4..0x00187BE8 loads a0..a3 and $f12 for this call, but the
 * byte-matched func_001D9530 reads only $a0 (the light matrix); a1..a3/$f12
 * are never read by the callee (dead arguments kept so the call matches). */
extern void func_001D9530(void *light_mtx, void *unused_a1, void *unused_a2, void *unused_a3, float unused_f12);
extern void func_0021B9A0(int a0, float a1, float a2);
extern int *D_00275B40;
extern float D_700031B0[4]; /* scratchpad: func_0019A570 hit point */
extern float D_008105D0[4]; /* [4] keeps it absolute under -sdatathreshold 4 */

void func_00187780(char *arg0, int arg1, int arg2) {
    float sp90[4];   /* 0x90 */
    float spA0[4];   /* 0xA0 */
    float spB0[4];   /* 0xB0 */
    float spC0[4];   /* 0xC0 */
    float spD0[4];   /* 0xD0 */
    float spE0[16];  /* 0xE0: 4x4 light matrix; rows at +0x10/+0x20/+0x30 */
    float sp120[16]; /* 0x120: 4x4 rotation matrix */
    float size;
    float r;
    float len2;
    float f22, f23, f20;
    float dist;
    float f21;
    long long word64; /* $s1: packed word handed to func_00187690 in $t0 */

    switch (arg2) {
    case 0:
        switch (arg1) {
        case 0:
        case 1:
            size = 64.0f;
            f22 = 17.0f;
            f23 = 3.5f;
            word64 = 0x20045D05554221F6LL;
            spB0[2] = 16.0f;
            spB0[1] = 16.0f;
            spB0[0] = 16.0f;
            r = 5.0f + (3.0f * ((float) func_00122BB8() / 2.1474836e9f));
            spC0[2] = r;
            spC0[1] = r;
            spC0[0] = r;
            break;
        }
        break;
    case 1:
        switch (arg1) {
        case 0:
        case 1:
            size = 32.0f;
            f22 = 17.0f;
            f23 = 3.5f;
            word64 = 0x20048D0599422050LL;
            spB0[2] = 8.0f;
            spB0[1] = 8.0f;
            spB0[0] = 8.0f;
            r = 2.0f + ((float) func_00122BB8() / 2.1474836e9f);
            spC0[2] = r;
            spC0[1] = r;
            spC0[0] = r;
            break;
        }
        break;
    }

    func_001029C0(spE0);
    func_00102948(spE0, (char *) D_00275B40[0] + 0x90);
    func_00102948(&spE0[4], (char *) D_00275B40[0] + 0xA0);
    func_00102948(&spE0[8], (char *) D_00275B40[0] + 0xB0);
    func_001029C0(sp120);
    func_00102BB0(sp120, sp120, 1.5707964f);
    func_001026D0(spE0, spE0, sp120);
    func_00102918(spE0, spE0, arg0 + 0xB0);

    sp90[0] = 0.0f;
    sp90[1] = 0.0f;
    sp90[2] = 250.0f;
    sp90[3] = 1.0f;
    func_001026A0(sp90, spE0, sp90);

    if (func_0019A570(&spE0[12], sp90, 6, 0) != 0) {
        func_001028D0(sp90, D_700031B0, &spE0[12]);
        len2 = sp90[0] * sp90[0] + sp90[1] * sp90[1] + sp90[2] * sp90[2];
        f21 = f20 = func_0011E748(len2);
        if (!(f20 <= 17.0f)) {
            f20 = 17.0f;
        }
        if (!(f21 <= 250.0f)) {
            f21 = 250.0f;
        }
        f20 = f20 / f22;
        func_00102948(spD0, (char *) *(int *) 0x700031D0 + 0x24);
    } else {
        f21 = 250.0f;
        f20 = 1.0f;
        func_00102900(spD0, &spE0[8], -1.0f);
    }

    f22 = f22 * f20;
    f23 = f23 * f20;
    size = size * f20;

    if (arg2 == 0) {
        func_001028D0(sp90, &spE0[12], D_008105D0);
        func_00102760(sp90, sp90);
        func_00102760(spA0, &spE0[8]);
        dist = func_0011DF78(func_00102738(sp90, spA0));
        if (!(dist <= 0.7f)) {
            func_00103230(spB0, spB0, 1.0f - (0.8f * ((dist - 0.7f) / 0.3f)));
        }
    }

    func_0021B9A0(2, 0.0f, -140.0f);
    func_0021B9A0(3, 0.0f, 450.5f);
    func_00187690(spE0, spB0, spC0, float_to_int(size), word64, f22, 0.1f, f23);
    if (!(func_001B0070() & 0x20000000)) {
        func_001D9530(spE0, spB0, spC0, spD0, f21);
    }
    if (arg2 == 1) {
        func_00103230(spB0, spB0, 2.0f);
        func_00103230(spC0, spC0, 1.0f);
        func_00187690(spE0, spB0, spC0, float_to_int(size), 0x20045D05554221F6LL, f22, 0.1f, f23 * 0.75f);
    }
    func_0021B9A0(1, 0.0f, 0.0f);
}
