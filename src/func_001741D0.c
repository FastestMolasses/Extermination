// NEARMISS func_001741D0  (vram 0x001741D0, 0x614 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.02% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 97.018 (mwcc233; 93.21 on 991202, 93.50 on mwcc24). The jtbl dispatch itself MATCHES (lui %hi(@45)/sll/addu/lw/jr and sltiu at,a1,6 all diff-clean; no reloc residual). 12 residual rows, all idiom-13 delay-slot-fill + one dead-dup, none logical: (a) 9 conditional-branch delay slots where CW 2.3.1 ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: per-frame update for one actor (arg0), split into two independent
// state machines selected by the mode byte arg0[0x1F0]:
//   mode 0x38 -> a 6-state jr-table switch on arg0[6] (jtbl_0026D6E0, sltiu 6),
//     followed by a common tail (arg0+0xB4 -= 0.2; func_00175900(arg0, 1)):
//       0: advance, clear arg0[7], func_001749A0(arg0, 2, 1, 0), clear arg0[0x302].
//       1: build the approach vector at D_700038A0 from arg0+0x290 towards
//          arg0+0xB0 (func_001028D0); its planar length
//          sqrt(x*x + z*z) (func_0011E748) is cached at 0x70003A20. Within 1.0
//          unit: snap arg0+0xB0/+0xB8 to arg0+0x290/+0x298, func_001749A0(...,4.0)
//          and advance. Otherwise normalise it, build the up vector at
//          D_700038D0 = (0,0,1,0), cross/blend through func_001026A0 +
//          func_00103200(0.5) + func_00102760, nudge arg0+0xB0/+0xB8 by
//          0.4 * the result and set the yaw arg0+0xC4 = func_0011E620(x, z).
//       2: ease the yaw arg0+0xC4 toward arg0+0x218 by 0.20943952 rad/frame
//          (func_001B12B0); once they are equal, advance and
//          func_001749A0(arg0, 0x170, 0, 8.0).
//       3: on arg0+0x200 & 0x1000: func_001749A0(arg0, 0x171, 0, 1.0), advance,
//          arg0[0x1F1] = 1, arg0[0] = 1.
//       4: when the global button mask D_00810E74 & the pad word at 0x70003B76:
//          advance, arg0[0x1F1] = 2, func_001749A0(arg0, 0x172, 0, 1.0),
//          arg0[0] |= 2. Else, if arg0[0x302] is set, arg0[0xF] = 6,
//          arg0[0] |= 2, arg0[0x1F1] = 2.
//       5: on arg0+0x200 & 0x1000: reset arg0[5]/[6]/[0x1F0], func_00174A50(16.0),
//          arg0[0] = 1.
//   otherwise -> a 4-way compare chain on arg0[6] (3, 2, 1, 0 descending):
//       0: advance, clear arg0[7], snap arg0+0xB0/B4/B8 to arg0+0x290/294/298,
//          then set arg0+0xC4 = func_001B1470(arg0+0x218 +/- pi/2) depending on
//          the u16 at arg0+0x2E, func_001749A0(0x173 / 0x174) and clear +0x21C.
//       1: on arg0+0x200 & 0x1000: advance, arg0[0x1F1] = 1, arg0[0] = 1,
//          arg0+0xC4 = arg0+0x218, func_001749A0(0x175). Else accumulate the
//          frame delta arg0+0x38 = (*D_00275B40)[2] - arg0+0x21C, latch +0x21C
//          and func_00178B90(arg0, 0).
//       2: if arg0+0x224 or +0x22C are non-zero, or arg0[0xF] & 2, force the
//          fallback pose (arg0[4]=2, [5]=0x15, [6]=0, [0x1F1]=2,
//          arg0+0xB0/B4/B8 = 99.8/63.0/88.5). Otherwise, on the pad mask:
//          advance, func_001749A0(0x176, 1.0), arg0[0x1F1]=2, arg0[0] |= 2,
//          clear +0x21C.
//       3: on arg0+0x200 & 0x1000: reset and func_00174A50(16.0), arg0[0] = 1.
//          Else advance along the yaw arg0+0xC4 by the frame delta
//          (*D_00275B40)[0]: arg0+0xB0 += dt*cos, arg0+0xB8 -= dt*sin.
extern void func_001028D0(void *a0, void *a1, void *a2);
extern void func_00102760(void *a0, void *a1);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_00103200(void *a0, void *a1, void *a2, float f0);
extern float func_0011E748(float f0);
extern float func_0011E620(float f0, float f1);
extern float func_0011DE90(float f0);
extern float func_0011E2A8(float f0);
extern float func_001B12B0(float f0, float f1, float f2);
extern float func_001B1470(float f0);
extern void func_001749A0(void *a0, int a1, int a2, float f0);
extern void func_00174A50(void *a0, float f0);
extern void func_00175900(void *a0, int a1);
extern void func_00178B90(void *a0, int a1);
extern int D_700038A0[8];
extern int D_700038B0[8];
extern int D_700038C0[8];
extern int D_700038D0[8];
extern unsigned short D_00810E74[4];
extern float **D_00275B40;

void func_001741D0(unsigned char *arg0) {
    float d;
    float az;
    float ax;
    float zero;
    int st;

    if (arg0[0x1F0] == 0x38) {
        st = arg0[6];
        switch (st) {
        case 0:
            arg0[6] = *(volatile unsigned char *)(arg0 + 6) + 1;
            arg0[7] = 0;
            func_001749A0(arg0, 2, 1, 0.0f);
            arg0[0x302] = 0;
            break;
        case 1:
            func_001028D0(D_700038A0, arg0 + 0x290, arg0 + 0xB0);
            ax = *(volatile float *)0x700038A0;
            az = *(volatile float *)0x700038A8;
            d = func_0011E748(ax * ax + az * az);
            *(volatile float *)0x70003A20 = d;
            if (d <= 1.0f) {
                *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x290);
                *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x298);
                func_001749A0(arg0, 0, 0, 4.0f);
                arg0[6] = arg0[6] + 1;
            } else {
                func_00102760(D_700038A0, D_700038A0);
                *(volatile int *)0x700038D0 = 0;
                *(volatile int *)0x700038D4 = 0;
                *(volatile int *)0x700038D8 = 0x3F800000;
                *(volatile int *)0x700038DC = 0;
                func_001026A0(D_700038B0, arg0 + 0xD0, D_700038D0);
                func_00103200(D_700038C0, D_700038A0, D_700038B0, 0.5f);
                func_00102760(D_700038C0, D_700038C0);
                *(float *)(arg0 + 0xB0) += 0.4f * *(volatile float *)0x700038C0;
                *(float *)(arg0 + 0xB8) += 0.4f * *(volatile float *)0x700038C8;
                *(float *)(arg0 + 0xC4) =
                    func_0011E620(*(volatile float *)0x700038C0, *(volatile float *)0x700038C8);
            }
            break;
        case 2:
            *(float *)(arg0 + 0xC4) =
                func_001B12B0(*(float *)(arg0 + 0x218), *(float *)(arg0 + 0xC4), 0.20943952f);
            if (*(float *)(arg0 + 0x218) == *(float *)(arg0 + 0xC4)) {
                arg0[6] = arg0[6] + 1;
                func_001749A0(arg0, 0x170, 0, 8.0f);
            }
            break;
        case 3:
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                func_001749A0(arg0, 0x171, 0, 1.0f);
                arg0[6] = arg0[6] + 1;
                arg0[0x1F1] = 1;
                arg0[0] = 1;
            }
            break;
        case 4:
            if (D_00810E74[0] & *(volatile unsigned short *)0x70003B76) {
                arg0[6] = st + 1;
                arg0[0x1F1] = 2;
                func_001749A0(arg0, 0x172, 0, 1.0f);
                arg0[0] = arg0[0] | 2;
            } else if (arg0[0x302] != 0) {
                arg0[0xF] = 6;
                arg0[0] = arg0[0] | 2;
                arg0[0x1F1] = 2;
            }
            break;
        case 5:
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                arg0[5] = 0;
                arg0[6] = 0;
                arg0[0x1F0] = 0;
                func_00174A50(arg0, 16.0f);
                arg0[0] = 1;
            }
            break;
        }
        *(float *)(arg0 + 0xB4) += -0.2f;
        func_00175900(arg0, 1);
        return;
    }
    st = arg0[6];
    switch (st) {
    case 0:
        arg0[6] = st + 1;
        arg0[7] = 0;
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x290);
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x294);
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x298);
        if (*(unsigned short *)(arg0 + 0x2E) == 0) {
            *(float *)(arg0 + 0xC4) = func_001B1470(1.5707964f + *(float *)(arg0 + 0x218));
            func_001749A0(arg0, 0x173, 0, 0.0f);
            *(int *)(arg0 + 0x21C) = 0;
        } else {
            *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0x218) - 1.5707964f);
            func_001749A0(arg0, 0x174, 0, 0.0f);
            *(int *)(arg0 + 0x21C) = 0;
        }
        return;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[6] = st + 1;
            arg0[0x1F1] = 1;
            arg0[0] = 1;
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x218);
            func_001749A0(arg0, 0x175, 0, 0.0f);
            return;
        }
        *(float *)(arg0 + 0x38) = (*D_00275B40)[2] - *(float *)(arg0 + 0x21C);
        *(float *)(arg0 + 0x21C) = (*D_00275B40)[2];
        func_00178B90(arg0, 0);
        return;
    case 2:
        zero = 0.0f;
        if (*(float *)(arg0 + 0x224) != zero || *(float *)(arg0 + 0x22C) != zero ||
            (arg0[0xF] & 2)) {
            arg0[4] = 2;
            arg0[5] = 0x15;
            arg0[6] = 0;
            arg0[0x1F1] = 2;
            *(int *)(arg0 + 0xB0) = 0x42C7999A;
            *(int *)(arg0 + 0xB4) = 0x427C0000;
            *(int *)(arg0 + 0xB8) = 0x42B10000;
            return;
        }
        if (D_00810E74[0] & *(volatile unsigned short *)0x70003B76) {
            arg0[6] = st + 1;
            func_001749A0(arg0, 0x176, 0, 1.0f);
            arg0[0x1F1] = 2;
            arg0[0] = arg0[0] | 2;
            *(int *)(arg0 + 0x21C) = 0;
        }
        return;
    case 3:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[5] = 0;
            arg0[6] = 0;
            arg0[0x1F0] = 0;
            func_00174A50(arg0, 16.0f);
            arg0[0] = 1;
            return;
        }
        *(float *)(arg0 + 0x38) = (*D_00275B40)[0] - *(float *)(arg0 + 0x21C);
        *(float *)(arg0 + 0x21C) = (*D_00275B40)[0];
        *(float *)(arg0 + 0xB0) +=
            *(float *)(arg0 + 0x38) * func_0011DE90(*(float *)(arg0 + 0xC4));
        *(float *)(arg0 + 0xB8) -=
            *(float *)(arg0 + 0x38) * func_0011E2A8(*(float *)(arg0 + 0xC4));
        return;
    }
}
