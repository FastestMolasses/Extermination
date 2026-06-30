// NEARMISS func_001FBF50  (vram 0x001FBF50, 0x328 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring + 2-instruction delay-slot scheduling near-miss. Body/structure fully recovered and logically identical. The two residual diffs are the arg-setup/delay-slot fill order on the 2nd func_00102948 call (target puts a1=arg0+0xB0 in the jal delay slot and computes a0=&D_70003610 be...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 99.87 (mwcc 2.3.3; pinned 991202 = 88.74). Body and control flow
// fully recovered; logic verified against the splat disassembly. The sole
// residual is a 2-instruction delay-slot scheduling permutation on the second
// func_00102948 call (target fills the jal slot with a1 = arg0+0xB0 and sets
// up a0 = &D_70003610 beforehand; mwcc emits the two arg setups in the
// opposite order). FP-coloring of k/w was driven to match by declaring k
// before w. Boot ELF stays byte-identical via the splat .s.
//
// Approach-and-retreat distance driver. Sets *arg1=*arg2=0, builds two scratch
// matrices from D_00810360 and arg0+0xB0 (func_00102948), optionally clears two
// scratchpad words when arg3!=0, then measures the distance via func_001B15D0.
// Returns 0 if dist >= fparg0 (out of range). Otherwise computes a cosine-eased
// magnitude `scaled = fparg1 * cos-ish(pi/2 * (fparg0-dist)/fparg0)`, rebuilds a
// transform chain (func_00102C58/func_001026A0/func_001028D0/func_00102760),
// derives a weight w (=dist/18 clamped to 1.0), and a 5th-power signed easing k.
// When D_0028215B==1 it writes the same scaled int to both outputs; otherwise it
// splits scaled and scaled*k between *arg1/*arg2 based on func_001B1380's side test.

extern int float_to_int(float);
extern void func_001026A0(float *, float *, float *);
extern float func_00102738(float *, float *);
extern void func_00102760(float *, float *);
extern void func_001028D0(float *, float *, float *);
extern void func_00102948(float *, float *);
extern void func_001029C0(float *);
extern void func_00102C58(float *, float *, float *);
extern float func_0011E2A8(float);
extern int func_001B1380(char *, float *, float);
extern float func_001B15D0(float *, float *);
extern unsigned char D_0028215B;
extern float D_0081027C;
extern float D_00810360;
extern float D_008105D0;
extern float D_70003400;
extern float D_70003600;
extern float D_70003610;

int func_001FBF50(char *arg0, int *arg1, int *arg2, char arg3, float fparg0, float fparg1) {
    float dist;
    float scaled;
    float k;
    float t;
    float p;
    float w;
    int iv;

    *arg2 = 0;
    *arg1 = 0;
    func_00102948(&D_70003600, &D_00810360);
    func_00102948(&D_70003610, (float *)(arg0 + 0xB0));
    if (arg3 != 0) {
        *(int *)0x70003614 = 0;
        *(int *)0x70003604 = 0;
    }
    dist = func_001B15D0(&D_70003600, &D_70003610);
    if (!(dist < fparg0)) {
        return 0;
    }
    scaled = fparg1 * func_0011E2A8(1.5707964f * ((fparg0 - dist) / fparg0));
    func_001029C0(&D_70003400);
    *(int *)0x70003600 = 0;
    *(float *)0x70003604 = D_0081027C;
    *(int *)0x70003608 = 0;
    *(int *)0x7000360C = 0x3F800000;
    func_00102C58(&D_70003400, &D_70003400, &D_70003600);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(int *)0x70003608 = 0x3F800000;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(&D_70003600, &D_70003400, &D_70003600);
    *(float *)0x70003610 = *(float *)(arg0 + 0xB0);
    *(float *)0x70003614 = *(float *)(arg0 + 0xB4);
    *(float *)0x70003618 = *(float *)(arg0 + 0xB8);
    *(float *)0x7000361C = *(float *)(arg0 + 0xBC);
    func_001028D0(&D_70003610, &D_70003610, &D_008105D0);
    *(int *)0x70003614 = 0;
    func_00102760(&D_70003610, &D_70003610);
    if (dist <= 18.0f) {
        w = 0.055555556f * dist;
    } else {
        w = 1.0f;
    }
    if (D_0028215B == 1) {
        iv = float_to_int(scaled);
        *arg2 = iv;
        *arg1 = iv;
        return 1;
    }
    t = func_00102738(&D_70003600, &D_70003610);
    p = t * t;
    p = p * p;
    k = t * p;
    k = k * w;
    if (k < 0.0f) {
        k = k - (1.0f - w);
    } else {
        k = k + (1.0f - w);
    }
    if (func_001B1380((char *)(arg0 + 0xB0), &D_008105D0, D_0081027C) != 0) {
        *arg1 = float_to_int(scaled);
        *arg2 = float_to_int(scaled * k);
    } else {
        *arg2 = float_to_int(scaled);
        *arg1 = float_to_int(scaled * k);
    }
    return 1;
}
