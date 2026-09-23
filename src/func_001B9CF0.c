// func_001B9CF0 -- byte-matched from C (objdiff 100%). Jump-table dispatcher: the
// compiled local .rodata table is pinned at its original address
// (tools/decomp/rodata_pin.py). Promoted from NEARMISS in the jr-table lane
// (2026-09-23): the arrival flag is `flag = (a == b);` instead of
// `flag = 1; if (a != b) flag = 0;` (idiom-35).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: one interpolation step for camera-tween channel arg2->chan (int at
// arg2+8, 0..13); returns 1 when the channel has reached its target this step.
// Each channel eases its current value toward its target with func_001B12B0(target,
// current, rate) where rate = arg2[0xC], then reports equality:
//   0-2: arg0 position component n     (arg0+0xC0+n*4)  vs arg2+0x20+n*4
//   3:   all three position components at once; matches only when all three land
//   4-6: arg1 look-at component n      (arg0+0xB0+n*4)  vs arg1+0x30+n*4
//   7-9: global camera-angle slot      D_00810354[n]    vs arg2+0x04+n*4
//   10:  unused (never completes)
//   11-13: field-of-view, two-phase (sub-state byte arg1[4]): phase 0 latches the
//        target into arg2+0x10 via func_001B1470, phase 1 eases arg0+0x94+n*4 to it.

extern float D_00810354[];

extern float func_001B12B0(float, float, float);
extern float func_001B1470(float);

int func_001B9CF0(unsigned char *arg0, unsigned char *arg1, unsigned char *arg2)
{
    int n;
    int o;
    int flag;
    int cnt;
    int sub;
    float res;
    float fa;

    n = *(int *)(arg2 + 8);
    switch (n) {
    case 0:
    case 1:
    case 2:
        o = n * 4;
        res = func_001B12B0(*(float *)(o + (int)arg2 + 0x20),
                            *(float *)(o + (int)arg0 + 0xC0),
                            *(float *)(arg2 + 0xC));
        o = *(int *)(arg2 + 8) * 4;
        *(float *)(o + (int)arg0 + 0xC0) = res;
        o = *(int *)(arg2 + 8) * 4;
        flag = 1;
        if (*(float *)(o + (int)arg0 + 0xC0) != *(float *)(o + (int)arg2 + 0x20)) {
            flag = 0;
        }
        if (flag != 0) {
            return 1;
        }
        break;
    case 7:
    case 8:
    case 9:
        o = n * 4;
        res = func_001B12B0(*(float *)(o + (int)arg2 + 4), D_00810354[n],
                            *(float *)(arg2 + 0xC));
        D_00810354[*(int *)(arg2 + 8)] = res;
        n = *(int *)(arg2 + 8);
        o = n * 4;
        flag = (D_00810354[n] == *(float *)(o + (int)arg2 + 4));
        if (flag != 0) {
            return 1;
        }
        break;
    case 3:
        cnt = 0;
        *(float *)(arg0 + 0xC0) = func_001B12B0(*(float *)(arg2 + 0x20),
                                                *(float *)(arg0 + 0xC0),
                                                *(float *)(arg2 + 0xC));
        if (*(float *)(arg0 + 0xC0) == *(float *)(arg2 + 0x20)) {
            cnt = 1;
        }
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg2 + 0x24),
                                                *(float *)(arg0 + 0xC4),
                                                *(float *)(arg2 + 0xC));
        if (*(float *)(arg2 + 0x24) == *(float *)(arg0 + 0xC4)) {
            cnt += 1;
        }
        *(float *)(arg0 + 0xC8) = func_001B12B0(*(float *)(arg2 + 0x28),
                                                *(float *)(arg0 + 0xC8),
                                                *(float *)(arg2 + 0xC));
        if (*(float *)(arg2 + 0x28) == *(float *)(arg0 + 0xC8)) {
            cnt += 1;
        }
        if (cnt == 3) {
            return 1;
        }
        break;
    case 4:
    case 5:
    case 6:
        o = n * 4;
        res = func_001B12B0(*(float *)(o + (int)arg1 + 0x30),
                            *(float *)(o + (int)arg0 + 0xB0),
                            *(float *)(arg2 + 0xC));
        o = *(int *)(arg2 + 8) * 4;
        *(float *)(o + (int)arg0 + 0xB0) = res;
        o = *(int *)(arg2 + 8) * 4;
        flag = 1;
        if (*(float *)(o + (int)arg1 + 0x30) != *(float *)(o + (int)arg0 + 0xB0)) {
            flag = 0;
        }
        if (flag != 0) {
            return 1;
        }
        break;
    case 11:
    case 12:
    case 13:
        sub = arg1[4];
        switch (sub) {
        case 0:
            arg1[4] = sub + 1;
            o = *(int *)(arg2 + 8) * 4;
            *(float *)(arg2 + 0x10) =
                func_001B1470(*(float *)(o + (int)arg0 + 0x94) +
                              *(float *)(o + (int)arg2 - 0xC));
            break;
        case 1:
            o = n * 4;
            res = func_001B12B0(*(float *)(arg2 + 0x10),
                                *(float *)(o + (int)arg0 + 0x94),
                                *(float *)(arg2 + 0xC));
            o = *(int *)(arg2 + 8) * 4;
            *(float *)(o + (int)arg0 + 0x94) = res;
            o = *(int *)(arg2 + 8) * 4;
            fa = *(float *)(o + (int)arg0 + 0x94);
            if (fa != *(float *)(arg2 + 0x10)) {
                break;
            }
            return 1;
        }
        break;
    }
    return 0;
}
