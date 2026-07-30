// NEARMISS func_00184D20  (vram 0x00184D20, 0x6FC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 10 instructions in two independent spots. (1) 6 instrs -- FP temp PAIR coloring for the first `sub.s` of the b[8]==1 and b[8]==2 blocks. Target: `lwc1 $f2,0xA0(s2); lwc1 $f1,0xA8(s2); lwc1 $f3,0(v0); lwc1 $f0,8(v0); sub.s $f20,$f2,$f3`; mwcc swaps only the f2/f3 pair: `lwc1 $f3,0xA0(s2) ... lwc1 ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_0011DF78(float);
extern float func_0011E620(float, float);
extern float func_0011E748(float);
extern float func_001B1470(float);

int func_00184D20(unsigned char *a, unsigned char *b, int ev) {
    int type;
    unsigned char k;
    float ang;
    float dx;
    float dy;
    float dz;
    float *q;

    type = (unsigned char)(b[2] & ~0xE0);
    if (type == 4) {
        switch (ev) {
        case 0x20:
            if (b[3] != 0x36) {
                return 0;
            }
            break;
        case 0x27:
            if (b[3] != 0x35) {
                return 0;
            }
            break;
        case 0x25:
            if (b[3] != 0x21) {
                return 0;
            }
            break;
        case 0x26:
            if (b[3] != 0x49) {
                return 0;
            }
            break;
        case 0x1B:
        case 0x1C:
        case 0x1D:
            k = b[3];
            if (k != 0x14 && k != 0x22 && k != 0x23 && k != 0x24 && k != 0x25 && k != 0x26 && k != 0x2C) {
                return 0;
            }
            break;
        default:
            return 0;
        }
    } else {
        if (type != 6) {
            return 0;
        }
        switch (ev) {
        case 0x23:
            k = b[3];
            if (k != 0x2F && k != 0x12) {
                return 0;
            }
            break;
        case 0x24:
            k = b[3];
            if (k != 0x13 && k != 0x12 && k != 0x2F) {
                return 0;
            }
            break;
        case 0x1B:
        case 0x1C:
        case 0x1D:
            k = b[3];
            if (k != 0x14 && k != 0x22 && k != 0x23 && k != 0x24 && k != 0x25 && k != 0x26 && k != 0x2C) {
                return 0;
            }
            break;
        case 0x26:
        case 0x28:
        default:
            return 0;
        }
    }

    switch (b[8]) {
    case 0:
    default:
        dx = *(float *)(a + 0xA0) - *(float *)(b + 0xB0);
        dy = *(float *)(a + 0xA8) - *(float *)(b + 0xB8);
        if (!(func_0011E748(dx * dx + dy * dy) <= (*(float **)(b + 0x30))[0])) {
            return 0;
        }
        dz = *(float *)(a + 0xA4) - *(float *)(b + 0xB4);
        if (!(func_0011E748(dz * dz) <= (*(float **)(b + 0x30))[1])) {
            return 0;
        }
        ang = func_001B1470(3.1415927410125732f + *(float *)(a + 0xC4) - *(float *)(b + 0xC4));
        break;
    case 1:
        q = *(float **)(b + 0x30);
        dx = *(float *)(a + 0xA0) - q[0];
        dy = *(float *)(a + 0xA8) - q[2];
        if (!(func_0011E748(dx * dx + dy * dy) <= (*(float **)(b + 0x30))[3])) {
            return 0;
        }
        q = *(float **)(b + 0x30);
        dz = *(float *)(a + 0xA4) - q[1];
        if (!(func_0011E748(dz * dz) <= (*(float **)(b + 0x30))[4])) {
            return 0;
        }
        if (type == 0xA) {
            ang = func_001B1470(*(float *)(a + 0xC4) - func_0011E620(-dx, -dy));
        } else {
            ang = func_001B1470(3.1415927410125732f + *(float *)(a + 0xC4) - (*(float **)(b + 0x30))[5]);
        }
        break;
    case 2:
        q = *(float **)(b + 0x30);
        dx = q[0] - *(float *)(a + 0xA0);
        dy = q[2] - *(float *)(a + 0xA8);
        if (!(func_0011E748(dx * dx + dy * dy) <= (*(float **)(b + 0x30))[3])) {
            return 0;
        }
        dz = *(float *)(a + 0xA4) - *(float *)(b + 0xB4);
        if (!(func_0011E748(dz * dz) <= (*(float **)(b + 0x30))[4])) {
            return 0;
        }
        ang = 0.0f;
        break;
    case 3:
    case 4: {
        float ex = *(float *)(b + 0xB0) - *(float *)(a + 0xA0);
        float ey = *(float *)(b + 0xB8) - *(float *)(a + 0xA8);
        float ed = func_0011E748(ex * ex + ey * ey);
        q = *(float **)(b + 0x30);
        if (!(ed <= q[0])) {
            return 0;
        }
        dz = *(float *)(a + 0xA4) - *(float *)(b + 0xB4);
        if (dz >= 0.0f) {
            if (!(dz <= q[1])) {
                return 0;
            }
        } else if (!(func_0011DF78(dz) <= 17.0f + (*(float **)(b + 0x30))[1])) {
            return 0;
        }
        if (b[8] == 4) {
            ang = func_001B1470(*(float *)(a + 0xC4) - *(float *)(b + 0xC4));
        } else if (ed <= 7.0f) {
            ang = 0.0f;
        } else {
            ang = func_001B1470(*(float *)(a + 0xC4) - func_0011E620(ex, ey));
        }
        if (func_0011DF78(ang) <= 1.5707963705062866f) {
            return 1;
        }
        return 0;
    }
    case 5:
        dx = *(float *)(b + 0xB0) - *(float *)(a + 0xA0);
        dy = *(float *)(b + 0xB8) - *(float *)(a + 0xA8);
        if (!(dx * dx + dy * dy <= 196.0f)) {
            return 0;
        }
        if (func_0011DF78(*(float *)(b + 0xB4) - *(float *)(a + 0xA4)) <= 4.0f) {
            return 1;
        }
        return 0;
    }
    if (func_0011DF78(ang) <= 0.7853981852531433f) {
        return 1;
    }
    return 0;
}
