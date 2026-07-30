// NEARMISS func_001B41F0  (vram 0x001B41F0, 0x61C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.73% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 94.734 (mwcc233; 80.95 on 991202). IMPORTANT: the old src stub's three-wall verdict is largely DISPROVEN - the jtbl dispatch, the whole case tree, the case-1/9 deflect legs, the case-13 reflected-spray leg, the case-19 mailbox leg, the saved-register mapping a0..t1 -> s6,s5,s4,s3,s2,s1, the two `...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS (verified against the disassembly, s85): the HIT APPLICATION --
// func_001B41F0(victim, hitPos, dir, flags, p5, p6) -> 1 if the hit was applied,
// 0 if it was deflected. Dispatches on the victim model byte victim[3] through
// jtbl_0026DDF0 (bound sltiu 0x14; out-of-range -> FX id 0x80000007).
//   case 1  : if victim[0xD] == 3 and (victim[5] == 9 or the player yaw
//             D_00810354 < 47.0), just deflect -- func_00102948(victim+0x70,
//             dir) and return 0. Otherwise fall through to FX 0x80000024.
//   6,7     : FX 0x80000024.   2: FX 0x80000025.
//   10,11   : FX 0x80000035.   3: FX 0x80000034.
//   13      : reflected spray -- build the reflection at D_70003610 from
//             D_00810350 and victim+0xB0 (func_001028D0), normalise it
//             (func_00102760), set w = 1.0, copy hitPos into D_70003600
//             (func_001031E0) and spawn FX 0x80000026; return 1 WITHOUT writing
//             the damage mailbox.
//   9       : FX 0x80000025, but if flags & 0x40 deflect and return 0.
//   19      : victim[0] = 3, func_001031E0(victim+0x70, hitPos), write the
//             mailbox victim+0x36 = flags ? ((p5 | p6*5) | 0x8000) : (p6 | p5);
//             return 1.
//   0,16,17,18 and the default/FX legs share the same "wide" body:
//             zero D_70003600, set +0x04 = sin(pi + D_00810374) via
//             func_001B1470, +0x08 = 0, +0x0C = 1.0; draw three random bytes
//             (func_00122BB8 >> 17 / >> 13 / >> 19, & 0xFF, / 255.0) into the
//             scratch triple at 0x70003680/84/88; scatter the impact point
//             D_70003610..18 = (hitPos[i] - 0.5) + spray[i], w = 1.0; orient
//             with func_00102948(victim+0x70, dir) and set victim[0] = 3.
//             Armoured leg (flags set): mailbox |= 0x8000, FX 0x80000076 and
//             sound 0x15D @300. Unarmoured leg: FX id (0x80000007 for the
//             0/16/17/18 copy, the per-model id otherwise), mailbox = p6 | p5,
//             then a ricochet sound 0x15A/0x15B chosen by rand bit 13 -- the
//             per-model copy first checks (short)p5 & 0x1000 and plays 0x1B1
//             instead.
extern void func_00102948(void *a0, void *a1);
extern void func_001028D0(void *a0, void *a1, void *a2);
extern void func_00102760(void *a0, void *a1);
extern void func_001031E0(void *a0, void *a1);
extern void func_001EFD90(int a0, void *a1, void *a2);
extern int func_00122BB8();
extern void func_001FBD50(void *a0, int a1, int a2, float f0);
extern void func_001FC580(void *a0, int a1);
extern float func_001B1470(float f0);
extern float D_00810354;
extern float D_00810374;
extern int D_00810350;
extern int D_70003600;
extern int D_70003610;

int func_001B41F0(unsigned char *victim, float *hitPos, void *dir, int flags, int p5, int p6) {
    int fx;
    float sa;
    float sb;
    float sc;

    switch (victim[3]) {
    case 1:
        if (victim[0xD] == 3) {
            if (victim[5] == 9 || D_00810354 < 47.0f) {
                func_00102948(victim + 0x70, dir);
                return 0;
            }
        }
        /* fall through */
    case 6:
    case 7:
        fx = 0x80000024;
        break;
    case 2:
        fx = 0x80000025;
        break;
    case 10:
    case 11:
        fx = 0x80000035;
        break;
    case 3:
        fx = 0x80000034;
        break;
    case 13:
        func_001028D0(&D_70003610, &D_00810350, victim + 0xB0);
        func_00102760(&D_70003610, &D_70003610);
        *(volatile int *)0x7000361C = 0x3F800000;
        func_001031E0(&D_70003600, hitPos);
        func_001EFD90(0x80000026, &D_70003600, &D_70003610);
        return 1;
    case 9:
        fx = 0x80000025;
        if (flags & 0x40) {
            func_00102948(victim + 0x70, dir);
            return 0;
        }
        break;
    case 19:
        victim[0] = 3;
        func_001031E0(victim + 0x70, hitPos);
        if (flags) {
            *(short *)(victim + 0x36) = (p5 | (p6 * 5)) | 0x8000;
        } else {
            *(short *)(victim + 0x36) = p6 | p5;
        }
        return 1;
    case 0:
    case 16:
    case 17:
    case 18:
        *(volatile int *)0x70003600 = 0;
        *(volatile float *)0x70003604 = func_001B1470(3.1415927f + D_00810374);
        *(volatile int *)0x70003608 = 0;
        *(volatile int *)0x7000360C = 0x3F800000;
        *(volatile float *)0x70003680 = (float)((func_00122BB8() >> 17) & 0xFF) / 255.0f;
        *(volatile float *)0x70003684 = (float)((func_00122BB8() >> 13) & 0xFF) / 255.0f;
        *(volatile float *)0x70003688 = (float)((func_00122BB8() >> 19) & 0xFF) / 255.0f;
        sa = *(float *)0x70003680;
        sb = *(float *)0x70003684;
        sc = *(float *)0x70003688;
        *(volatile float *)0x70003610 = (hitPos[0] - 0.5f) + sa;
        *(volatile float *)0x70003614 = (hitPos[1] - 0.5f) + sb;
        *(volatile float *)0x70003618 = (hitPos[2] - 0.5f) + sc;
        *(volatile int *)0x7000361C = 0x3F800000;
        func_00102948(victim + 0x70, dir);
        victim[0] = 3;
        if (flags) {
            *(short *)(victim + 0x36) = (p5 | (p6 * 5)) | 0x8000;
            func_001EFD90(0x80000076, &D_70003610, &D_70003600);
            func_001FBD50(victim, 0x15D, 0, 300.0f);
        } else {
            func_001EFD90(0x80000007, &D_70003610, &D_70003600);
            *(short *)(victim + 0x36) = p6 | p5;
            if ((func_00122BB8() >> 13) & 1) {
                func_001FC580(victim, 0x15A);
            } else {
                func_001FC580(victim, 0x15B);
            }
        }
        return 1;
    default:
        fx = 0x80000007;
        break;
    }
    *(volatile int *)0x70003600 = 0;
    *(volatile float *)0x70003604 = func_001B1470(3.1415927f + D_00810374);
    *(volatile int *)0x70003608 = 0;
    *(volatile int *)0x7000360C = 0x3F800000;
    *(volatile float *)0x70003680 = (float)((func_00122BB8() >> 17) & 0xFF) / 255.0f;
    *(volatile float *)0x70003684 = (float)((func_00122BB8() >> 13) & 0xFF) / 255.0f;
    *(volatile float *)0x70003688 = (float)((func_00122BB8() >> 19) & 0xFF) / 255.0f;
    sa = *(float *)0x70003680;
    sb = *(float *)0x70003684;
    sc = *(float *)0x70003688;
    *(volatile float *)0x70003610 = (hitPos[0] - 0.5f) + sa;
    *(volatile float *)0x70003614 = (hitPos[1] - 0.5f) + sb;
    *(volatile float *)0x70003618 = (hitPos[2] - 0.5f) + sc;
    *(volatile int *)0x7000361C = 0x3F800000;
    func_00102948(victim + 0x70, dir);
    victim[0] = 3;
    if (flags) {
        *(short *)(victim + 0x36) = (p6 | p5) | 0x8000;
        func_001EFD90(0x80000076, &D_70003610, &D_70003600);
        func_001FBD50(victim, 0x15D, 0, 300.0f);
    } else {
        func_001EFD90(fx, &D_70003610, &D_70003600);
        *(short *)(victim + 0x36) = p6 | p5;
        if ((short)p5 & 0x1000) {
            func_001FBD50(victim, 0x1B1, 0, 300.0f);
        } else if ((func_00122BB8() >> 13) & 1) {
            func_001FC580(victim, 0x15A);
        } else {
            func_001FC580(victim, 0x15B);
        }
    }
    return 1;
}
