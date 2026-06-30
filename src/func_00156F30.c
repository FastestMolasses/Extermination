// NEARMISS func_00156F30  (vram 0x00156F30, 0x430 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.50% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-register-half coloring + scheduling near-miss (91.50% on mwcc 2.3.3; logic fully recovered). The 4-state camera/look-at driver (states 0,1,4 + default 3), all float math, the gp-relative D_00275B40[0]/[1] table accesses, the absolute 0x7000xxxx scratch globals, and the func-ptr tailcall (arg0+...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (objdiff 91.50% vs expected on mwcc 2.3.3; 90.28% on pinned 991202).
// Logic fully recovered: a 4-state look-at/camera-spin driver dispatched on the
// state byte at arg0+4 (gp-relative D_00275B40 holds the two active camera/root
// pointers [0] and [1]; the 0x7000xxxx addresses are scratch-vector globals).
// State 0: once func_001B0FD0 settles, set arg0=1, snapshot the two camera roots
// (+0x90) into the local rig at arg0+0x1F0 and +0x40 via copy_qw4, advance to
// state 4. State 4: if the arg0+0x36 trigger is set, re-enter state 1, reset the
// spin params (0x2E8=0.2, 0x2E4=0), rebuild the orientation (func_001028D0/
// func_001029C0/func_00102BB0) and pick the spin sign (+/-0.08) from the staged
// vector at 0x700038A0; then run func_001B17A0 and the per-object update through
// the function pointer at arg0+0x4C. State 1: advance the spin angle 0x2E4 by
// 0x2E0, clamp the rig angle field at +0xF4 to [-pi,pi], scale by 0x2E8 and a
// func_0011E2A8 lookup into 0x70003A20, rotate the orientation, apply it to both
// camera roots (func_001026D0), offset the secondary root by 4*func_0011E398 of
// the angle, decay the 0xF8 timer by 0.001 (->state 4 at zero), and on the
// arg0+0x36 retrigger reset like state 4 but writing the spin sign as a fixed
// 0x3DA3D70A/0xBDA3D70A at +0xF0; close with func_001B17A0 + the arg0+0x4C call.
// Default state 3: func_001AFC10(3).
//
// Residual is FP-register-half coloring (target odd-half fv0f/fs0f vs mwcc
// even-half), the state-selector reg color, and branch-likely sense -- all
// permuter/coloring territory, not a clean-store delay-slot nop.
extern int func_001B0FD0(int a);
extern void func_001C6380(void *p);
extern void copy_qw4(void *dst, void *src);
extern void func_001028D0(void *a, void *b, void *c);
extern void func_001029C0(void *a);
extern void func_00102BB0(void *a, void *b, float t);
extern void func_001026A0(void *a, void *b, void *c);
extern float func_0011E2A8(float x);
extern void func_00102A60(void *a, void *b, float t);
extern void func_001026D0(void *a, void *b, void *c);
extern float func_0011E398(float x);
extern void func_001B17A0(void *p);
extern void func_001AFC10(unsigned char h);
extern char **D_00275B40;
extern char D_00810350[];
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_00156F30(char *arg0) {
    char *cam;
    char *pf4;
    char *p2e0;
    char *p2e8;

    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        if (func_001B0FD0(1) == 0) {
            *(char *)(arg0 + 0) = 1;
            func_001C6380(arg0);
            cam = arg0 + 0x1F0;
            copy_qw4(cam, D_00275B40[0] + 0x90);
            copy_qw4(cam + 0x40, D_00275B40[1] + 0x90);
            *(unsigned char *)(arg0 + 4) = 4;
        }
        return;
    case 4:
        if (*(short *)(arg0 + 0x36) != 0) {
            *(unsigned char *)(arg0 + 4) = 1;
            *(float *)(arg0 + 0x2E8) = 0.2f;
            *(float *)(arg0 + 0x2E4) = 0.0f;
            *(short *)(arg0 + 0x36) = 0;
            func_001028D0(D_700038A0, arg0 + 0xB0, D_00810350);
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, -*(float *)(arg0 + 0xC4));
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            if (*(float *)0x700038A0 < 0.0f) {
                *(float *)(arg0 + 0x2E0) = 0.08f;
            } else {
                *(float *)(arg0 + 0x2E0) = -0.08f;
            }
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    case 1:
        cam = arg0 + 0x1F0;
        *(float *)(arg0 + 0x2E4) = *(float *)(arg0 + 0x2E0) + *(float *)(arg0 + 0x2E4);
        pf4 = cam + 0xF4;
        p2e0 = cam + 0xF0;
        p2e8 = cam + 0xF8;
        if (*(float *)(arg0 + 0x2E4) > 3.1415927f) {
            *(int *)pf4 = 0xC0490FDB;
        } else if (*(float *)(arg0 + 0x2E4) < -3.1415927f) {
            *(int *)pf4 = 0x40490FDB;
        }
        *(float *)0x70003A20 = *(float *)(arg0 + 0x2E8) * func_0011E2A8(*(float *)pf4);
        func_001029C0(D_700036A0);
        func_00102A60(D_700036A0, D_700036A0, *(float *)0x70003A20);
        func_001C6380(arg0);
        func_001026D0(D_00275B40[0] + 0x90, D_00275B40[0] + 0x90, D_700036A0);
        func_001026D0(D_00275B40[1] + 0x90, D_00275B40[1] + 0x90, D_700036A0);
        *(int *)0x700038AC = 0;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038A4 = 0;
        *(float *)0x700038A0 = 4.0f * func_0011E398(*(float *)0x70003A20);
        func_001026A0(D_700038B0, D_00275B40[1] + 0x90, D_700038A0);
        *(float *)(D_00275B40[1] + 0xC0) = *(float *)(D_00275B40[1] + 0xC0) - *(float *)0x700038B0;
        *(float *)(D_00275B40[1] + 0xC8) = *(float *)(D_00275B40[1] + 0xC8) - *(float *)0x700038B8;
        *(float *)p2e8 = *(float *)p2e8 - 0.001f;
        if (*(float *)p2e8 < 0.0f) {
            *(unsigned char *)(arg0 + 4) = 4;
        }
        if (*(short *)(arg0 + 0x36) != 0) {
            *(unsigned char *)(arg0 + 4) = 1;
            *(float *)p2e8 = 0.2f;
            *(short *)(arg0 + 0x36) = 0;
            func_001028D0(D_700038A0, arg0 + 0xB0, D_00810350);
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, -*(float *)(arg0 + 0xC4));
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            if (*(float *)0x700038A0 < 0.0f) {
                *(int *)p2e0 = 0x3DA3D70A;
            } else {
                *(int *)p2e0 = 0xBDA3D70A;
            }
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    case 3:
        func_001AFC10(3);
        break;
    }
}
