// NEARMISS func_0022E7F0  (vram 0x0022E7F0, 0x3E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.03% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc233 packs the three stack-scratch locals by SIZE (largest array always lowest offset) rather than by declaration/usage order; the resulting constant stack-offset shift accounts for most of the diff, plus one independent list-scheduler interleaving difference in the 6-field rescale block that ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS 80.0% (mwcc 2.3.3; pinned 991202 build 75.1%). Logic fully recovered.
//
// Radar/HUD camera-relative setup. arg0 is the viewer entity. Dispatches
// func_00207D00(1,0), then runs func_0022E630(arg0, blip) over the live blip
// list (D_00275B58 count / D_00275B54 array, skipping empty-string entries).
// Builds a scratch transform D_700036E0 (0.8/1.0/0.5/0 packed floats), computes
// the signed heading delta between arg0's forward vector and a fixed +240
// offset vector via func_0011E620 (atan2-like) normalized into (-pi,pi], turns
// that into a yaw-only rotation matrix (func_001029C0/func_00102BB0), combines
// it into D_700036A0, seeds a camera-frustum quad (96,0,340,1.0) transformed by
// func_001031E0, copies three D_0026A9x0 corner vectors into D_700038Ax0,
// rotates them by D_700036A0 (func_001026A0), rescales x/y/z/u/v/w by
// 16*(1792+v) / 16*(1936+v/2), and finally hands the block to func_00208040
// plus three func_002082B0(1, D_0026Axxxx) HUD-icon calls.
//
// WALL: body/structure/all globals/constants fully recovered (including
// fixing the func_001749A0-style over-decl idiom for the D_0026A9C0/AA20/AA80
// pointer globals so they keep absolute %hi/%lo addressing under
// -sdatathreshold 4, needed for the D_00275B58/D_00275B54 gp-rel loop counter
// pair). Sole residual: mwcc233 packs the three stack locals (a 4x4 scratch
// matrix + two vec3 scratch buffers) by SIZE, always placing the largest
// array at the lowest stack offset, regardless of declaration/usage order --
// confirmed genuine by sweeping the scratch-matrix declared size from 4 to 20
// floats and every declaration permutation, which always reproduced the same
// lowest-offset placement (target instead places locals in usage order,
// increasing with offset). This is a mwcc-vs-CW stack-slot-allocation-order
// wall (frame-stride class); it shifts every subsequent stack-relative
// instruction by a constant delta, which is the majority of the residual
// diff, plus one independent list-scheduler interleaving difference in the
// six-field x/y/z/u/v/w rescale block (target software-pipelines all six
// expressions sharing four live FP constants across statement boundaries;
// mwcc233 computes them per-statement) that could not be reproduced by
// reordering the six store statements.
extern void func_00207D00(int a, int b);
extern void func_0022E630(char *arg0, char *arg1);
extern void func_001029C0(void *mtx);
extern void func_00102948(void *dst, void *src);
extern void func_00102760(void *dst, void *src);
extern float func_0011E620(float dx, float dy);
extern void func_00102BB0(void *dst, void *src, float angle);
extern void func_001026D0(void *dst, void *a, void *b);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001031E0(void *dst, void *src);
extern void func_00208040(int a, void *b, void *c, void *d);
extern void func_002082B0(int a, void *b);

extern short D_00275B58;
extern char **D_00275B54;
extern float D_0026A990[2];
extern float D_0026A9A0[2];
extern float D_0026A9B0[2];
extern char D_0026A9C0[8];
extern char D_0026AA20[8];
extern char D_0026AA80[8];
extern float D_700036A0[3];
extern float D_700036D0[3];
extern float D_700036E0[3];
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038C0;

void func_0022E7F0(char *arg0) {
    int cnt;
    char **list;
    float ang0;
    float ang1;
    float da;
    float sp70[16];
    float sp60[3];
    int sp64;
    float sp50[3];
    int sp54;

    func_00207D00(1, 0);

    cnt = D_00275B58;
    list = D_00275B54;
    if (cnt != 0) {
        do {
            char *e = *list;
            cnt--;
            list++;
            if (*(unsigned char *)e != 0) {
                func_0022E630(arg0, e);
            }
        } while (cnt != 0);
    }

    func_001029C0(D_700036E0);
    *(int *)0x700036E0 = 0x3F4CCCCD;
    *(int *)0x700036F4 = 0x3F800000;
    *(int *)0x70003708 = 0x3F000000;
    *(int *)0x7000371C = 0;

    func_00102948(sp60, (char *)*(int *)(arg0 + 0x12C) + 0x90);
    sp64 = 0;
    func_00102760(sp60, sp60);
    ang0 = func_0011E620(sp60[2], sp60[0]);

    func_00102948(sp50, arg0 + 0xF0);
    sp54 = 0;
    func_00102760(sp50, sp50);
    ang1 = func_0011E620(sp50[2], sp50[0]);

    da = -(ang0 - ang1);
    if (da <= -3.1415927f) {
        do {
            da += 6.2831855f;
        } while (da <= -3.1415927f);
    }
    if (!(da <= 3.1415927f)) {
        do {
            da = da - 6.2831855f;
        } while (!(da <= 3.1415927f));
    }

    func_001029C0(sp70);
    func_00102BB0(sp70, sp70, da);
    func_001026D0(D_700036E0, sp70, D_700036E0);

    func_001029C0(D_700036A0);
    func_001026D0(D_700036A0, D_700036E0, D_700036A0);

    *(float *)0x700038A0 = 96.0f;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = 340.0f;
    *(int *)0x700038AC = 0x3F800000;
    func_001031E0(D_700036D0, &D_700038A0);

    func_00102948(&D_700038A0, D_0026A990);
    func_00102948(&D_700038B0, D_0026A9A0);
    func_00102948(&D_700038C0, D_0026A9B0);

    func_001026A0(&D_700038A0, D_700036A0, &D_700038A0);
    func_001026A0(&D_700038B0, D_700036A0, &D_700038B0);
    func_001026A0(&D_700038C0, D_700036A0, &D_700038C0);

    *(float *)0x700038A0 = 16.0f * (1792.0f + *(float *)0x700038A0);
    *(float *)0x700038A8 = 16.0f * (1936.0f + (*(float *)0x700038A8 / 2.0f));
    *(float *)0x700038B0 = 16.0f * (1792.0f + *(float *)0x700038B0);
    *(float *)0x700038C0 = 16.0f * (1792.0f + *(float *)0x700038C0);
    *(float *)0x700038B8 = 16.0f * (1936.0f + (*(float *)0x700038B8 / 2.0f));
    *(float *)0x700038C8 = 16.0f * (1936.0f + (*(float *)0x700038C8 / 2.0f));

    func_00208040(1, &D_700038A0, &D_700038B0, &D_700038C0);
    func_002082B0(1, D_0026A9C0);
    func_002082B0(1, D_0026AA20);
    func_002082B0(1, D_0026AA80);
}
