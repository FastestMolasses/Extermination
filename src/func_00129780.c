// NEARMISS func_00129780  (vram 0x00129780, 0x77C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.57% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 3 instructions, one site. At the FIRST of the six func_001C2540 axis probes (target 0x001299C0 `bnez $v0, .L00129B4C` + `nop`) mwcc-2.3.3 speculatively hoists `lui at, 0x7000` into the bnez delay slot and retargets the branch one instruction further in (0x3d0 vs 0x3cc), then re-emits the lui on t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: Per-frame move/collide step for a character (a = actor at arg0,
// b = its motion/render block at arg1), dispatched by a move-type selector
// (arg2 & 0xFF) through jtbl_0026CFA0 (13 entries). Returns 1 when the actor
// finished/advanced its animation slot, 0 otherwise.
//   0,1,5,6   simple forward probe func_001C2430(a, b+0x60, a+0xD0); on a hit set
//             result state 1, else decay the Y accumulator *(float*)(a+0xB4) by 1.
//   2,7       build the vector (-3,1,0,1) at scratchpad 0x700038A0, transform it by
//             the actor matrix (func_001026A0), add the actor position a+0xB0..0xB8,
//             and test it with func_0019AD00(..., 0x80000007). On a hit state 2;
//             otherwise repeat with (+3,1,0,1) and on a hit state 2, else decay
//             *(float*)(a+0xB0) by 1.
//   3,4,8,9   six axis probes in order (0,-6,0) (0,6,0) (6,0,0) (-6,0,0) (0,0,6)
//             (0,0,-6) through func_001C2540; the first one that reports a blocker
//             reads the camera/target flags at *(short*)(*0x700031D0 + 0x1A):
//             0x3800 -> state 2, else 0x8000 -> state 3, else state 1. If none of
//             the six hit, decay *(float*)(a+0xB4) by 1.
//   10,11     commit a turn: a[4]=1, a[5]=0xC, clear a[6]/a[7], set the quaternion
//             (0,-1,0,1) at b+0x80, build (0,1,0,1) in scratchpad 0x70003610, run
//             func_001C3DB0/func_001031E0 into b+0x70, reset b+0x80 to (0,1,0,1),
//             func_001C3BE0, copy the matrix (copy_qw4 a+0xD0 <- 0x70003000), set
//             b+0xE4=0x500, b+0xD8=0, b+0xF0=0, kick func_00128830(a, 0, 5, 0.5)
//             and func_001287F0(a, b, 0xF, 0), and set *(float*)(a+0xC0) = pi/2.
//             Returns 1.
//   12        same shape with a[5]=0xD, an extra copy_qw4(0x70003400 <- 0x70003000),
//             func_001C69A0(a), b+0xE4=0x400, b+0xF0=0. Returns 1.
// Tail (also the default arm): bump the u16 timer at a+0x28; once it passes 0x28
// reset it, set a[4]=3, a[5]=0, b[0xE0]=0 and return 0. Otherwise, if a state was
// raised, advance a[4], clear a[5], re-seat the orientation from the camera at
// *0x700031D0 (+0x24/0x28/0x2C) through func_001C3DB0/func_001031E0/func_001C3BE0,
// copy the matrix, push it back by -4 (func_00103230 + func_001028B8), run one more
// probe with the (0,-8,0) offset, store state<<8 into b+0xE4 and return 1; else 0.
//
// NEARMISS 99.572 with mwcc 2.3.3 (mwcps2-2.3.3-000906) -O4,p -sdatathreshold 0.
// SOLE RESIDUAL (3 instrs): at the FIRST of the six func_001C2540 probes mwcc
// speculatively hoists the `lui at, 0x7000` that both successors need (the next
// probe's scratchpad store and the shared blocked-path load of *0x700031D0) into
// the `bnez v0` delay slot and branches one instruction further in; CW leaves the
// delay slot a nop and rebuilds the lui. The other five probes match exactly.
// This is the documented idiom-13 delay-slot-fill wall (a global address `lui`
// exposed in a successor is never matchable from C). Tried: volatile on the
// 0x700031D0 pointer, fully-nested ifs instead of goto, -O3,p / -O4 / -O4,s and
// mwcc 2.3.1 (89.5%) - all no better.
extern void copy_qw4(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern void func_001028B8(void *, void *, void *);
extern void func_001031E0(void *, void *);
extern void func_00103230(void *, void *, float);
extern void func_001287F0(void *, void *, int, float);
extern void func_00128830(void *, float, float, float);
extern int func_0019AD00(void *, void *, unsigned int);
extern int func_001C2430(void *, void *, void *);
extern int func_001C2540(void *, void *, void *, void *);
extern void func_001C3BE0(void *, void *);
extern void func_001C3DB0(void *, void *, void *, void *);
extern void func_001C69A0(void *);

extern float D_70003000;
extern float D_70003400;
extern float D_70003600;
extern float D_70003610;
extern float D_70003620;
extern float D_700038A0;
extern float D_700038B0;

int func_00129780(unsigned char *a, unsigned char *b, unsigned char sel) {
    int st;
    short flags;
    char *cam;

    st = 0;
    switch (sel) {
    case 0:
    case 1:
    case 5:
    case 6:
        if (func_001C2430(a, b + 0x60, a + 0xD0) != 0) {
            st = 1;
        } else {
            *(float *)(a + 0xB4) -= 1.0f;
        }
        break;
    case 2:
    case 7:
        *(float *)0x700038A0 = -3.0f;
        *(float *)0x700038A4 = 1.0f;
        *(float *)0x700038A8 = 0.0f;
        *(float *)0x700038AC = 1.0f;
        func_001026A0(&D_700038A0, a + 0xD0, &D_700038A0);
        *(float *)0x700038A0 += *(float *)(a + 0xB0);
        *(float *)0x700038A4 += *(float *)(a + 0xB4);
        *(float *)0x700038A8 += *(float *)(a + 0xB8);
        if (func_0019AD00(a, &D_700038A0, 0x80000007) != 0) {
            st = 2;
        } else {
            *(float *)0x700038A0 = 3.0f;
            *(float *)0x700038A4 = 1.0f;
            *(float *)0x700038A8 = 0.0f;
            *(float *)0x700038AC = 1.0f;
            func_001026A0(&D_700038A0, a + 0xD0, &D_700038A0);
            *(float *)0x700038A0 += *(float *)(a + 0xB0);
            *(float *)0x700038A4 += *(float *)(a + 0xB4);
            *(float *)0x700038A8 += *(float *)(a + 0xB8);
            if (func_0019AD00(a, &D_700038A0, 0x80000007) != 0) {
                st = 2;
            } else {
                *(float *)(a + 0xB0) -= 1.0f;
            }
        }
        break;
    case 3:
    case 4:
    case 8:
    case 9:
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = -6.0f;
        *(float *)0x700038A8 = 0.0f;
        *(float *)0x700038AC = 1.0f;
        if (func_001C2540(a, b + 0x60, &D_700038A0, a + 0xD0) != 0) {
            goto blocked;
        }
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = 6.0f;
        *(float *)0x700038A8 = 0.0f;
        *(float *)0x700038AC = 1.0f;
        if (func_001C2540(a, b + 0x60, &D_700038A0, a + 0xD0) != 0) {
            goto blocked;
        }
        *(float *)0x700038A0 = 6.0f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = 0.0f;
        *(float *)0x700038AC = 1.0f;
        if (func_001C2540(a, b + 0x60, &D_700038A0, a + 0xD0) != 0) {
            goto blocked;
        }
        *(float *)0x700038A0 = -6.0f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = 0.0f;
        *(float *)0x700038AC = 1.0f;
        if (func_001C2540(a, b + 0x60, &D_700038A0, a + 0xD0) != 0) {
            goto blocked;
        }
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = 6.0f;
        *(float *)0x700038AC = 1.0f;
        if (func_001C2540(a, b + 0x60, &D_700038A0, a + 0xD0) != 0) {
            goto blocked;
        }
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = -6.0f;
        *(float *)0x700038AC = 1.0f;
        if (func_001C2540(a, b + 0x60, &D_700038A0, a + 0xD0) != 0) {
            goto blocked;
        }
        *(float *)(a + 0xB4) -= 1.0f;
        break;
blocked:
        flags = *(short *)(*(char **)0x700031D0 + 0x1A);
        if (flags & 0x3800) {
            st = 2;
        } else if (flags & 0x8000) {
            st = 3;
        } else {
            st = 1;
        }
        break;
    case 10:
    case 11:
        a[4] = 1;
        a[5] = 0xC;
        a[6] = 0;
        a[7] = 0;
        *(float *)(b + 0x80) = 0.0f;
        *(float *)(b + 0x84) = -1.0f;
        *(float *)(b + 0x88) = 0.0f;
        *(float *)(b + 0x8C) = 1.0f;
        *(float *)0x70003610 = 0.0f;
        *(float *)0x70003614 = 1.0f;
        *(float *)0x70003618 = 0.0f;
        *(float *)0x7000361C = 1.0f;
        func_001C3DB0(b + 0x80, &D_70003610, b + 0x70, &D_70003620);
        func_001031E0(b + 0x70, &D_70003620);
        *(float *)(b + 0x80) = 0.0f;
        *(float *)(b + 0x84) = 1.0f;
        *(float *)(b + 0x88) = 0.0f;
        *(float *)(b + 0x8C) = 1.0f;
        func_001C3BE0(a, b);
        copy_qw4(a + 0xD0, &D_70003000);
        *(int *)(b + 0xE4) = 0x500;
        *(int *)(b + 0xD8) = 0;
        *(int *)(b + 0xF0) = 0;
        func_00128830(a, 0.0f, 5.0f, 0.5f);
        func_001287F0(a, b, 0xF, 0.0f);
        *(float *)(a + 0xC0) = 1.5707964f;
        return 1;
    case 12:
        a[4] = 1;
        a[5] = 0xD;
        a[6] = 0;
        a[7] = 0;
        *(float *)0x70003610 = 0.0f;
        *(float *)0x70003614 = 1.0f;
        *(float *)0x70003618 = 0.0f;
        *(float *)0x7000361C = 1.0f;
        func_001C3DB0(b + 0x80, &D_70003610, b + 0x70, &D_70003620);
        func_001031E0(b + 0x70, &D_70003620);
        *(float *)(b + 0x80) = 0.0f;
        *(float *)(b + 0x84) = 1.0f;
        *(float *)(b + 0x88) = 0.0f;
        *(float *)(b + 0x8C) = 1.0f;
        func_001C3BE0(a, b);
        copy_qw4(a + 0xD0, &D_70003000);
        copy_qw4(&D_70003400, &D_70003000);
        func_001C69A0(a);
        *(int *)(b + 0xE4) = 0x400;
        *(int *)(b + 0xF0) = 0;
        return 1;
    }
    *(short *)(a + 0x28) = *(short *)(a + 0x28) + 1;
    if (*(short *)(a + 0x28) > 0x28) {
        *(short *)(a + 0x28) = 0;
        a[4] = 3;
        a[5] = 0;
        b[0xE0] = 0;
        return 0;
    }
    if (st != 0) {
    a[4] = a[4] + 1;
    a[5] = 0;
    cam = *(char **)0x700031D0;
    *(float *)0x70003610 = *(float *)(cam + 0x24);
    *(float *)0x70003614 = *(float *)(cam + 0x28);
    *(float *)0x70003618 = *(float *)(cam + 0x2C);
    *(float *)0x7000361C = 1.0f;
    func_001C3DB0(b + 0x80, &D_70003610, b + 0x70, &D_70003620);
    func_001031E0(b + 0x70, &D_70003620);
    *(float *)(b + 0x80) = *(float *)(*(char **)0x700031D0 + 0x24);
    *(float *)(b + 0x84) = *(float *)(*(char **)0x700031D0 + 0x28);
    *(float *)(b + 0x88) = *(float *)(*(char **)0x700031D0 + 0x2C);
    *(float *)(b + 0x8C) = 1.0f;
    func_001C3BE0(a, b);
    copy_qw4(a + 0xD0, &D_70003000);
    func_00103230(&D_70003600, b + 0x80, -4.0f);
    *(float *)0x7000360C = 0.0f;
    func_001028B8(a + 0xB0, a + 0xB0, &D_70003600);
    *(float *)0x700038B0 = 0.0f;
    *(float *)0x700038B4 = -8.0f;
    *(float *)0x700038B8 = 0.0f;
    *(float *)0x700038BC = 1.0f;
    func_001C2540(a, b + 0x60, &D_700038B0, a + 0xD0);
    *(int *)(b + 0xE4) = st << 8;
    return 1;
    }
    return 0;
}
