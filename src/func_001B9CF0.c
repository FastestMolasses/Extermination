// NEARMISS func_001B9CF0  (vram 0x001B9CF0, 0x2FC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.54% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body is fully decoded and 178/193 instructions are byte-identical; the whole residual is ONE mwcc scheduling decision inside the case 7/8/9 (D_00810354) block. The target rematerializes %hi/%lo(D_00810354) TWICE up front, in the load-use shadow of the first `lw a0,0x8(s0)` (target order: lw a0,8(...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
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
        flag = 1;
        if (D_00810354[n] != *(float *)(o + (int)arg2 + 4)) {
            flag = 0;
        }
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
