// NEARMISS func_0017B910  (vram 0x0017B910, 0x330 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Branch-likely scheduling + FP register-coloring near-miss. Body and control flow fully recovered and logically identical (block layout, inverse-CSE recompute of the D_0024875C table addr, short sign-extension via long-long func_0017B490, and the dx*dx+dy*dy FPU MAC mula.s/madd.s all reproduce). R...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 80.55 (mwcc 2.3.3; pinned 991202 = 73.46). Body and control flow
// fully recovered; logic verified against the splat disassembly. Block layout
// (main 0x236==0 branch first, tail else), the inverse-CSE recompute of the
// D_0024875C clip-table address in the cur>=lim arm, the (short) sign-extension
// of func_0017B490's long-long return (dsll32/dsra32), and the dx*dx+dy*dy FPU
// MAC (mula.s/madd.s) all reproduce. Residuals are compiler artifacts: the
// target uses a branch-likely (bc1fl) on the cur<lim test where mwcc emits
// bc1f+nop; FP register coloring in the distance/MAC block differs (f2/f1 vs
// f0); and the tail func_001749A0 call's delay-slot/arg scheduling differs.
//
// Per-frame animation advance for an entity. Runs anim_eval_skeleton first.
// When the entity's 0x236 'busy' byte is 0 it advances the active clip: samples
// the clip frame via func_001C61D0/func_0017B490, eases the remaining time at
// scratchpad 0x70003A20.. against the clip-length table D_0024875C[state][sub],
// pulling end-pose data from the camera/track struct *D_00275B40 (+0x48 if not
// yet at the limit, else +0x44). For sub-clip 1 it halves the remaining time
// (min 1.0) into +0x268; otherwise sets +0x268=10 and kicks func_001749A0. It
// then computes the planar distance to target (dx=A28-posX, dy=A2C-posZ,
// dist=sqrt-ish func_0011E748(dx*dx+dy*dy)), builds an interpolation transform
// (func_001029C0/func_00102C58/func_001026A0), and stores per-axis step rates
// into +0x260/+0x264 (=transform / +0x268), marking state +0x1F0=5. When 0x236
// is set it instead just re-checks the current clip index: if changed it kicks
// func_001749A0 with 14.0; if unchanged and the 0x8000 flag is clear it clears
// the busy/sub state (+0x1F0=0, +0x25C=0).

extern void anim_eval_skeleton(char *);
extern void func_001026A0(float *, float *, float *);
extern void func_001029C0(float *);
extern void func_00102C58(float *, float *, char *);
extern float func_0011E748(float);
extern void func_001749A0(char *, short, int, float);
extern long long func_0017B490(char *, int, unsigned char, unsigned char);
extern int func_001C61D0(int, short);
extern char D_0024875C;
extern char *D_00275B40;
extern float D_700036A0;
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038B8;

void func_0017B910(char *arg0) {
    float cur;
    float lim;
    float t;
    char *gp;
    float dx, dy, dist;
    short r;

    anim_eval_skeleton(arg0);
    if (*(unsigned char *)(arg0 + 0x236) == 0) {
        *(float *)0x70003A20 = (float)func_001C61D0(*(int *)(arg0 + 0x40),
            func_0017B490(arg0, 1, *(unsigned char *)(arg0 + 0x235), *(unsigned char *)(arg0 + 0x25C)));
        cur = *(float *)(arg0 + 0x3C);
        lim = *(float *)((&D_0024875C + *(unsigned char *)(arg0 + 0x235) * 8) + *(unsigned char *)(arg0 + 0x25C) * 4);
        if (cur < lim) {
            gp = D_00275B40;
            *(float *)0x70003A24 = cur - 1.0f;
            *(float *)0x70003A20 = *(float *)0x70003A20 - 1.0f;
            *(float *)0x70003A28 = *(float *)(*(char **)(gp + 0x48) + 0xC0);
            *(float *)0x70003A2C = *(float *)(*(char **)(gp + 0x48) + 0xC8);
        } else {
            gp = D_00275B40;
            *(float *)0x70003A24 = cur - lim;
            *(float *)0x70003A20 = *(float *)0x70003A20 - *(float *)((&D_0024875C + *(unsigned char *)(arg0 + 0x235) * 8) + *(unsigned char *)(arg0 + 0x25C) * 4);
            *(float *)0x70003A28 = *(float *)(*(char **)(gp + 0x44) + 0xC0);
            *(float *)0x70003A2C = *(float *)(*(char **)(gp + 0x44) + 0xC8);
        }
        if (*(unsigned char *)(arg0 + 0x25C) == 1) {
            t = *(float *)0x70003A24 / 2.0f;
            *(float *)0x70003A24 = t;
            if (t < 1.0f) {
                *(float *)0x70003A24 = 1.0f;
            }
            *(float *)(arg0 + 0x268) = *(float *)0x70003A24;
        } else {
            *(float *)(arg0 + 0x268) = 10.0f;
            func_001749A0(arg0, func_0017B490(arg0, 6, *(unsigned char *)(arg0 + 0x235), *(unsigned char *)(arg0 + 0x25C)), 0,
                *(float *)(arg0 + 0x268));
        }
        dx = *(float *)0x70003A28 - *(float *)(arg0 + 0xB0);
        *(float *)0x70003A20 = dx;
        dy = *(float *)0x70003A2C - *(float *)(arg0 + 0xB8);
        *(float *)0x70003A24 = dy;
        dist = func_0011E748(dx * dx + dy * dy);
        *(float *)0x70003A28 = dist;
        func_001029C0(&D_700036A0);
        func_00102C58(&D_700036A0, &D_700036A0, arg0 + 0xC0);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(float *)0x700038A8 = *(float *)0x70003A28;
        *(int *)0x700038AC = 0;
        func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
        *(float *)(arg0 + 0x260) = *(float *)0x700038B0 / *(float *)(arg0 + 0x268);
        *(float *)(arg0 + 0x264) = *(float *)0x700038B8 / *(float *)(arg0 + 0x268);
        *(char *)(arg0 + 0x1F0) = 5;
    } else {
        r = func_0017B490(arg0, 0, *(unsigned char *)(arg0 + 0x235), 0);
        if (r == *(short *)(arg0 + 0x20C)) {
            if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
                *(char *)(arg0 + 0x1F0) = 0;
                *(unsigned char *)(arg0 + 0x25C) = 0;
            }
        } else {
            func_001749A0(arg0, r, 0, 14.0f);
        }
    }
}
