// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Perception / "can actor `a` sense event `ev` from sensor `b`?" predicate.
// Stage 1: gate on the sensor class in b[2]&0x1F (4 or 6) crossed with the
// event id `ev` and the sub-id b[3]; any unlisted pair returns 0.
// Stage 2: dispatch on the sensor shape b[8] and test `a`'s position against
// the shape's parameter block at *(float**)(b+0x30):
//   0 = sphere about b's own origin (b+0xB0/B4/B8), radii [0]=xz, [1]=y
//   1 = sphere about an explicit centre q[0..2], radii q[3]/q[4]
//   2 = same sphere, delta taken centre-minus-actor
//   3/4 = cone/box toward b, with a 17.0 slack below the plane
//   5 = fixed 14.0 xz radius, 4.0 y band -> immediate yes/no
// Shapes 0/1/2 fall through to the shared facing test: the residual angle
// must be within PI/4; shapes 3/4 use PI/2. func_0011E748=sqrtf,
// func_0011DF78=fabsf, func_0011E620=atan2f, func_001B1470=wrap to (-PI,PI].
//
// Matching notes (all three are load-bearing, do not "simplify"):
//  * case 5's tail must be spelled `if (fabs(dz) > 4.0f) return 0; return 1;`.
//    `if (... <= 4.0f) return 1; return 0;` and `if (!(... <= 4.0f)) return 0;
//    return 1;` both emit bc1f with the return-1 block first; only the `>`
//    form gives the target's bc1t with the return-0 block laid out first.
//  * case 1's dx/dy must dereference the parameter block as
//    `((float *)(*(int *)(b + 0x30)))[i]` and case 2's as
//    `(*(float **)(b + 0x30))[i]`. The int-load-then-cast form flips which
//    subtraction operand mwcc evaluates first, which is what colours the
//    first temp pair $f2/$f3 instead of $f3/$f2.
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
        dx = *(float *)(a + 0xA0) - ((float *)(*(int *)(b + 0x30)))[0];
        dy = *(float *)(a + 0xA8) - ((float *)(*(int *)(b + 0x30)))[2];
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
        dx = (*(float **)(b + 0x30))[0] - *(float *)(a + 0xA0);
        dy = (*(float **)(b + 0x30))[2] - *(float *)(a + 0xA8);
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
        if (func_0011DF78(*(float *)(b + 0xB4) - *(float *)(a + 0xA4)) > 4.0f) {
            return 0;
        }
        return 1;
    }
    if (func_0011DF78(ang) <= 0.7853981852531433f) {
        return 1;
    }
    return 0;
}
