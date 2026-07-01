// NEARMISS func_00178080  (vram 0x00178080, 0x220 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.59% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring permutation (body/logic/structure 100% correct): the target keeps the three loop pointers plus the loop count in t0/t1/t2 while mwcc assigns a4/a5/a6, plus a couple of prologue instruction-scheduling reorderings. Not the clean-store nop; 2.3.3 does not close it. Permuter territo...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 89.6% (mwcc 2.3.3; 991202 = 81.7%). Body/logic/structure fully
// recovered; the sole residual is a register-coloring permutation (the target
// keeps the three loop pointers + count in t0/t1/t2, mwcc assigns a4/a5/a6) plus
// a couple of prologue instruction-scheduling reorderings. Not the clean-store
// nop, so 2.3.3 does not close it -- permuter territory.
//
// Scans the active-slot table for the first slot whose sightline check passes,
// then aims at it. Prologue: fills the D_700038B0 target vector from camera
// state at *0x700031D0 (delta of 0x700031B0/B8 minus cam+0x24/+0x2C), sets
// self+0xB4+15 into 0x700038B4 and marks D_700038BC = 1.0f, then calls
// func_0019BC40(target, cam). Loop over 0..*0x700031E0: skip unless the u16 flag
// (D_70003170[i], &1) is set and the angle D_00282250[i] < 0.6283 (~36 deg). The
// window half-width is 6.0 if self+0xD == 2 else 3.0; if the stored angle 0x38B4
// lies inside (D_700030F0[i] - half, D_700030F0[i] + 6.0), lock on: write the
// world aim point into self+0x2E0/2E4/2E8 (cam pos + 1.5*cam basis, minus 20.5),
// compute the yaw via func_0011E620(-cam+0x2C, cam+0x24) into 0x70003A24, store
// self+0x218 = func_001B1470(4.712389 + yaw), and return 1. Return 0 if none.
extern float func_0011E620(float y, float x);
extern int func_0019BC40(void *p, void *q);
extern float func_001B1470(float a);

extern float D_00282250;
extern float D_700030F0;
extern unsigned short D_70003170;
extern float D_700038B0;

int func_00178080(char *arg0) {
    char *p;
    float f3;
    float lo;
    unsigned short *flags;
    float *tsin;
    float *tpos;
    int i;
    int cnt;

    p = *(char **)0x700031D0;
    *(float *)0x700038B0 = *(float *)0x700031B0 - *(float *)(p + 0x24);
    *(float *)0x700038B8 = *(float *)0x700031B8 - *(float *)(p + 0x2C);
    *(float *)0x700038B4 = 15.0f + *(float *)(arg0 + 0xB4);
    *(int *)0x700038BC = 0x3F800000;
    func_0019BC40(&D_700038B0, p);
    f3 = *(float *)0x700038B4;
    flags = &D_70003170;
    tsin = &D_00282250;
    tpos = &D_700030F0;
    cnt = *(int *)0x700031E0;
    i = 0;
    goto guard;
    for (;;) {
        if ((*flags & 1) && *tsin < 0.62831855f) {
            float six = 6.0f;
            float sel = (*(unsigned char *)(arg0 + 0xD) == 2) ? six : 3.0f;
            lo = *tpos - sel;
            if (!(f3 <= lo) && f3 < six + *tpos) {
                p = *(char **)0x700031D0;
                *(float *)(arg0 + 0x2E0) = *(float *)0x700031B0 + 1.5f * *(float *)(p + 0x24);
                *(float *)(arg0 + 0x2E8) = *(float *)0x700031B8 + 1.5f * *(float *)(*(char **)0x700031D0 + 0x2C);
                *(float *)(arg0 + 0x2E4) = (&D_700030F0)[i] - 20.5f;
                p = *(char **)0x700031D0;
                *(float *)0x70003A24 = func_0011E620(-*(float *)(p + 0x2C), *(float *)(p + 0x24));
                *(float *)(arg0 + 0x218) = func_001B1470(4.712389f + *(float *)0x70003A24);
                return 1;
            }
        }
        flags += 1;
        tsin += 1;
        tpos += 1;
        i++;
    guard:
        if (i >= cnt) {
            return 0;
        }
    }
}
