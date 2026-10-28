// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003600 0x70003604 0x70003608 0x7000360C 0x70003610 0x70003614 0x70003618 0x7000361C 0x70003680 0x70003684 0x70003688

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 94.73% NEARMISS).
// Fixes read from the original instructions:
//  - Every scratchpad word is a relocated extern (idiom-32, opted in with
//    // SPAD:), one extern per address. The FX origin / impact blocks
//    D_70003600..0C and D_70003610..1C are volatile (all their stores stay
//    in source order); the random spray triple D_70003680/84/88 is plain.
//  - The impact point adds the spray words directly
//    (`D_70003610 = (hitPos[0] - 0.5f) + D_70003680;`) instead of reading
//    them into locals first: with plain relocated externs the first two
//    spray loads move above the last spray store and the third is re-read
//    after it, exactly as in the original.

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
extern int func_00122BB8(void);
extern void func_001FBD50(void *a0, int a1, int a2, float f0);
extern void func_001FC580(void *a0, int a1);
extern float func_001B1470(float f0);
extern float D_00810354;
extern float D_00810374;
extern int D_00810350;
extern volatile float D_70003600;
extern volatile float D_70003604;
extern volatile float D_70003608;
extern volatile float D_7000360C;
extern volatile float D_70003610;
extern volatile float D_70003614;
extern volatile float D_70003618;
extern volatile float D_7000361C;
extern float D_70003680;
extern float D_70003684;
extern float D_70003688;

int func_001B41F0(unsigned char *victim, float *hitPos, void *dir, int flags, int p5, int p6) {
    int fx;

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
        D_7000361C = 1.0f;
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
        D_70003600 = 0.0f;
        D_70003604 = func_001B1470(3.1415927f + D_00810374);
        D_70003608 = 0.0f;
        D_7000360C = 1.0f;
        D_70003680 = (float)((func_00122BB8() >> 17) & 0xFF) / 255.0f;
        D_70003684 = (float)((func_00122BB8() >> 13) & 0xFF) / 255.0f;
        D_70003688 = (float)((func_00122BB8() >> 19) & 0xFF) / 255.0f;
        D_70003610 = (hitPos[0] - 0.5f) + D_70003680;
        D_70003614 = (hitPos[1] - 0.5f) + D_70003684;
        D_70003618 = (hitPos[2] - 0.5f) + D_70003688;
        D_7000361C = 1.0f;
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
    D_70003600 = 0.0f;
    D_70003604 = func_001B1470(3.1415927f + D_00810374);
    D_70003608 = 0.0f;
    D_7000360C = 1.0f;
    D_70003680 = (float)((func_00122BB8() >> 17) & 0xFF) / 255.0f;
    D_70003684 = (float)((func_00122BB8() >> 13) & 0xFF) / 255.0f;
    D_70003688 = (float)((func_00122BB8() >> 19) & 0xFF) / 255.0f;
    D_70003610 = (hitPos[0] - 0.5f) + D_70003680;
    D_70003614 = (hitPos[1] - 0.5f) + D_70003684;
    D_70003618 = (hitPos[2] - 0.5f) + D_70003688;
    D_7000361C = 1.0f;
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
