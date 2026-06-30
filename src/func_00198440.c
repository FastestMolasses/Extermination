// NEARMISS func_00198440  (vram 0x00198440, 0x204 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-coloring + scheduling near-miss. Body byte-identical after fixing the float const (0.4f=0x3ECCCCCD, NOT 0.1f), routing func_001026A0/func_00103230 first arg through the D_70003610 symbol (the float reloads stay raw 0x70003610), reordering the arg2==2 && r!=0 test, caching the three scratchpad ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Builds a transform/offset workspace in the EE scratchpad (0x70003600 region):
// writes (-D_002754E8, D_002754F0, D_002754EC, 0), calls func_001026A0 to populate
// D_70003610, then composes object position (arg0[4..6], [8..10]) from the entity
// at *(arg1+0x20)+0xA0.. and the scratchpad results. arg2 selects 3 path variants.
// NEARMISS 96.7% (mwcc 2.3.3): logic fully recovered & body byte-identical; sole
// residual is FPU register coloring (f2/f1/f0 vs f0/f1/f8) + one address-compute
// scheduled one slot early. Note 0.4f (0x3ECCCCCD) is the scale constant; the
// [16]/[4] array extern types are a matching device to pin sdata placement so
// D_002754E8/EC/F0 stay gp-rel while D_008105D0/E0/106C6/D_700038A0 use %hi/%lo.
extern int func_001026A0(float *, int, float *);
extern int func_0018C850(void *, float, float);
extern int func_0018C920(float *, void *, float);
extern int func_00102948(void *, float *);
extern int func_00103230(float *, float *, float);
extern int func_001028B8(float *, float *, float *);
extern int func_00198240(int, int);
extern float D_002754E8;
extern float D_002754EC;
extern float D_002754F0;
extern char D_008105D0[16];
extern char D_008105E0[16];
extern unsigned char D_008106C6[16];
extern float D_700038A0[4];
extern float D_70003600[4];
extern float D_70003610[4];

void func_00198440(float *arg0, int arg1, int arg2) {
    int s0 = *(int *)(arg1 + 0x20);
    int s1;
    float *sp = (float *)0x70003600;
    float f2 = -D_002754E8;
    float f1 = D_002754F0;
    float f0 = D_002754EC;

    sp[0] = f2;
    sp[1] = f1;
    sp[2] = f0;
    *(int *)0x7000360C = 0;
    s1 = s0 + 0x1F0;
    func_001026A0(D_70003610, *(int *)(s0 + 0x110) + 0x90, D_70003600);
    arg0[4] = *(float *)(s0 + 0xA0) + *(float *)0x70003610;
    arg0[5] = *(float *)(s0 + 0xA4) + *(float *)0x70003614;
    arg0[6] = *(float *)(s0 + 0xA8) + *(float *)0x70003618;
    if (arg2 == 0) {
        func_0018C850(D_008105D0, arg0[5], 0.4f);
        func_0018C920(arg0 + 4, D_008105D0, 0.4f);
    } else {
        func_00102948(D_008105D0, arg0 + 4);
    }
    if (*(int *)(s1 + 0x20) != 0 && D_008106C6[0] != 0) {
        func_00102948(arg0 + 8, (float *)(s1 + 0x10));
    } else {
        func_00103230(D_70003610, (float *)(s0 + 0xC0), 200.0f);
        arg0[8] = *(float *)(s0 + 0xA0) + *(float *)0x70003610;
        arg0[9] = *(float *)(s0 + 0xA4) + *(float *)0x70003614;
        arg0[10] = *(float *)(s0 + 0xA8) + *(float *)0x70003618;
    }
    func_00102948(D_008105E0, arg0 + 8);
    {
        int r = func_00198240(arg1, s0);
        if (arg2 == 2 && r != 0) {
            func_001028B8(arg0 + 4, arg0 + 4, D_700038A0);
            func_00102948(D_008105D0, arg0 + 4);
        }
    }
}
