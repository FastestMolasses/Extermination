// NEARMISS func_00183EF0  (vram 0x00183EF0, 0xCA8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.41% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 17 residual instructions / 810 (99.41%, mwcc 2.3.3, -O4,p -sdatathreshold 0). The jump-table dispatch itself matches 100% (lbu 0x8; sltiu 6; beql; lui/addiu %hi/%lo(jtbl_0026D810); sll; addu; lw; jr) and build/jtblrodata/func_00183EF0.s is present and correct — no jtbl reloc residual. Two residua...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: "can the player interact with / target this object?" predicate.
// arg0 = player entity, arg1 = candidate object. Called in a loop by
// func_00184BA0, which uses the return as: 0 = reject, 1 = candidate (the
// planar distance the test computed is left in the scratchpad float at
// 0x70003B98 so the caller can pick the nearest), 2 = take immediately.
//
// arg1 layout used here: +2 = flag/state byte (low 5 bits = "kind" after
// masking off 0xE0; low 4 bits compared against 7 separately), +3 = subtype,
// +8 = class selector (the jump-table switch, 0..5), +0x10 = think/update fn
// pointer, +0x30 = pointer to a 6-float tuning record
// {rad, height, x?, rad2, height2, yaw} (indices 0..5 as used below),
// +0xB0/+0xB4/+0xB8 = position xyz, +0xC0/+0xC4/+0xC8 = orientation
// (+0xC4 = yaw). arg0: +0xA0/+0xA4/+0xA8 = position xyz, +0xC4 = yaw,
// +0x1F0 = player state (0x2D = the special "45" state that takes the
// separate ray/dot-product path at the top).
//
// 0x7000xxxx are EE scratchpad workspaces: 0x70003600/0x70003610 = ray
// endpoints handed to func_0019A910 (the world raycast; returns nonzero on a
// hit, hit flags then read from *(short*)(*(int*)0x700031D0 + 0x1A)),
// 0x70003640/0x70003650/0x70003660 = vec4 scratch for the 0x2D path,
// 0x70003690/0x70003694 = its dot / squared-length results,
// 0x700031D4/0x700031D8 = the raycast's hit object / hit kind.
//
// Helper functions (named by observed use): func_00102738 = dot,
// func_00102760 = normalize, func_001028D0 = subtract, func_00102948 = copy,
// func_0011DE90 = cos, func_0011E2A8 = sin, func_0011DF78 = fabs,
// func_0011E620 = atan2, func_0011E748 = sqrt, func_001B1240 = yaw from a
// point to (x,z), func_001B1470 = wrap an angle to (-pi, pi].

extern float func_00102738(void *a, void *b);
extern void func_00102760(void *a, void *b);
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102948(void *a, void *b);
extern float func_0011DE90(float a);
extern float func_0011DF78(float a);
extern float func_0011E2A8(float a);
extern float func_0011E620(float a, float b);
extern float func_0011E748(float a);
extern int func_0019A910(void *a, void *b, int n);
extern float func_001B1240(void *a, float x, float z);
extern float func_001B1470(float a);
extern int func_00219550();

extern float D_008105E0[];
extern float D_70003600[];
extern float D_70003610[];
extern float D_70003640[];
extern float D_70003650[];
extern float D_70003660[];

int func_00183EF0(char *arg0, char *arg1)
{
    float dx;
    float dz;
    float dy;
    float dist;
    float ang;
    float *p;
    unsigned char kind;
    unsigned char sub;

    kind = *(unsigned char *)(arg1 + 2) & ~0xE0;

    if (*(unsigned char *)(arg0 + 0x1F0) == 0x2D) {
        if (kind != 7) {
            return 0;
        }
        func_00102948(D_70003640, arg0 + 0xA0);
        *(float *)0x70003644 += 1.0f;
        if (func_0019A910(D_70003640, arg1 + 0xB0, 6) != 0 &&
            (*(short *)(*(int *)0x700031D0 + 0x1A) & 0x2000) != 0) {
            return 0;
        }
        func_001028D0(D_70003640, D_008105E0, arg0 + 0xA0);
        *(int *)0x7000364C = 0x3F800000;
        func_00102760(D_70003640, D_70003640);
        func_001028D0(D_70003650, arg1 + 0xB0, arg0 + 0xA0);
        *(int *)0x7000365C = 0x3F800000;
        func_00102948(D_70003660, D_70003650);
        func_00102760(D_70003650, D_70003650);
        *(float *)0x70003690 = func_00102738(D_70003640, D_70003650);
        dist = func_00102738(D_70003660, D_70003660);
        *(float *)0x70003694 = dist;
        if (!(dist <= 144.0f)) {
            return 0;
        }
        if (dist < 4.0f) {
            return 2;
        }
        if (*(float *)0x70003690 < 0.0f) {
            return 0;
        }
        if (*(float *)0x70003690 < 0.4f && !(dist <= 9.0f)) {
            return 0;
        }
        return 2;
    }

    switch (*(unsigned char *)(arg1 + 8)) {
    default:
    case 0:
        if (kind == 5) {
            sub = *(unsigned char *)(arg1 + 3);
            if (sub == 3 || sub == 0x15) {
                dx = *(float *)(arg0 + 0xA0) -
                     (*(float *)(arg1 + 0xB0) - 5.0f * func_0011DE90(*(float *)(arg1 + 0xC4)));
                dz = *(float *)(arg0 + 0xA8) -
                     (*(float *)(arg1 + 0xB8) + 5.0f * func_0011E2A8(*(float *)(arg1 + 0xC4)));
                dist = func_0011E748(dx * dx + dz * dz);
                if (!(dist <= **(float **)(arg1 + 0x30))) {
                    return 0;
                }
                *(float *)0x70003B98 = dist;
                dy = *(float *)(arg0 + 0xA4) - *(float *)(arg1 + 0xB4);
                if (!(func_0011E748(dy * dy) <= (*(float **)(arg1 + 0x30))[1])) {
                    return 0;
                }
            } else {
                dx = *(float *)(arg0 + 0xA0) - *(float *)(arg1 + 0xB0);
                dz = *(float *)(arg0 + 0xA8) - *(float *)(arg1 + 0xB8);
                dist = func_0011E748(dx * dx + dz * dz);
                if (!(dist <= **(float **)(arg1 + 0x30))) {
                    return 0;
                }
                *(float *)0x70003B98 = dist;
                dy = *(float *)(arg0 + 0xA4) - *(float *)(arg1 + 0xB4);
                if (!(func_0011E748(dy * dy) <= (*(float **)(arg1 + 0x30))[1])) {
                    return 0;
                }
            }
            if (func_0011DF78(func_001B1470(func_001B1240(arg1 + 0xB0, *(float *)(arg0 + 0xA0),
                                                          *(float *)(arg0 + 0xA8)) -
                                            *(float *)(arg1 + 0xC4))) <= 1.5707964f) {
                ang = func_001B1470((3.1415927f + *(float *)(arg0 + 0xC4)) - *(float *)(arg1 + 0xC4));
            } else {
                ang = func_001B1470(*(float *)(arg0 + 0xC4) - *(float *)(arg1 + 0xC4));
            }
            break;
        }
        dx = *(float *)(arg0 + 0xA0) - *(float *)(arg1 + 0xB0);
        dz = *(float *)(arg0 + 0xA8) - *(float *)(arg1 + 0xB8);
        dist = func_0011E748(dx * dx + dz * dz);
        if (!(dist <= **(float **)(arg1 + 0x30))) {
            return 0;
        }
        *(float *)0x70003B98 = dist;
        dy = *(float *)(arg0 + 0xA4) - *(float *)(arg1 + 0xB4);
        if (!(func_0011E748(dy * dy) <= (*(float **)(arg1 + 0x30))[1])) {
            return 0;
        }
        p = *(float **)(arg1 + 0x30);
        if (kind == 4) {
            switch (*(unsigned char *)(arg1 + 3)) {
            case 0x37:
            case 0x38:
            case 0x14:
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
                ang = func_001B1470((3.1415927f + *(float *)(arg0 + 0xC4)) - *(float *)(arg1 + 0xC4));
                break;
            case 0x2C:
                *(float *)0x70003600 = *(float *)(arg0 + 0xA0);
                *(float *)0x70003604 = 16.0f + *(float *)(arg0 + 0xA4);
                *(float *)0x70003608 = *(float *)(arg0 + 0xA8);
                *(int *)0x7000360C = 0x3F800000;
                *(float *)0x70003610 = *(float *)(arg1 + 0xB0);
                *(float *)0x70003614 = 3.0f + *(float *)(arg1 + 0xB4);
                *(float *)0x70003618 = *(float *)(arg1 + 0xB8);
                *(int *)0x7000361C = 0x3F800000;
                if (func_0019A910(D_70003600, D_70003610, 4) != 0 &&
                    (*(short *)(*(int *)0x700031D0 + 0x1A) & 0x2800) != 0) {
                    return 0;
                }
                ang = func_001B1470((3.1415927f + *(float *)(arg0 + 0xC4)) - *(float *)(arg1 + 0xC4));
                break;
            default:
                ang = func_001B1470((3.1415927f + *(float *)(arg0 + 0xC4)) - p[2]);
                break;
            }
        } else if (kind == 10) {
            ang = func_001B1470(*(float *)(arg0 + 0xC4) - func_0011E620(-dx, -dz));
        } else if (kind == 6) {
            ang = func_001B1470((3.1415927f + *(float *)(arg0 + 0xC4)) - *(float *)(arg1 + 0xC4));
        } else {
            ang = func_001B1470((3.1415927f + *(float *)(arg0 + 0xC4)) - p[2]);
        }
        break;

    case 1:
        dx = *(float *)(arg0 + 0xA0) - (*(float **)(arg1 + 0x30))[0];
        dz = *(float *)(arg0 + 0xA8) - (*(float **)(arg1 + 0x30))[2];
        dist = func_0011E748(dx * dx + dz * dz);
        if (!(dist <= (*(float **)(arg1 + 0x30))[3])) {
            return 0;
        }
        *(float *)0x70003B98 = dist;
        dy = *(float *)(arg0 + 0xA4) - (*(float **)(arg1 + 0x30))[1];
        if (!(func_0011E748(dy * dy) <= (*(float **)(arg1 + 0x30))[4])) {
            return 0;
        }
        p = *(float **)(arg1 + 0x30);
        if (kind == 10) {
            ang = func_001B1470(*(float *)(arg0 + 0xC4) - func_0011E620(-dx, -dz));
        } else {
            ang = func_001B1470((3.1415927f + *(float *)(arg0 + 0xC4)) - p[5]);
        }
        break;

    case 2:
        dx = (*(float **)(arg1 + 0x30))[0] - *(float *)(arg0 + 0xA0);
        dz = (*(float **)(arg1 + 0x30))[2] - *(float *)(arg0 + 0xA8);
        dist = func_0011E748(dx * dx + dz * dz);
        if (!(dist <= (*(float **)(arg1 + 0x30))[3])) {
            return 0;
        }
        *(float *)0x70003B98 = dist;
        dy = *(float *)(arg0 + 0xA4) - *(float *)(arg1 + 0xB4);
        if (!(func_0011E748(dy * dy) <= (*(float **)(arg1 + 0x30))[4])) {
            return 0;
        }
        ang = 0.0f;
        break;

    case 3:
    case 4: {
        float bx;
        float bz;
        float bd;
        float bmax;

        bx = *(float *)(arg1 + 0xB0) - *(float *)(arg0 + 0xA0);
        bz = *(float *)(arg1 + 0xB8) - *(float *)(arg0 + 0xA8);
        bd = func_0011E748(bx * bx + bz * bz);
        if (!(bd <= **(float **)(arg1 + 0x30))) {
            return 0;
        }
        *(float *)0x70003B98 = bd;
        dy = *(float *)(arg0 + 0xA4) - *(float *)(arg1 + 0xB4);
        if (dy >= 0.0f) {
            if (!(dy <= (*(float **)(arg1 + 0x30))[1])) {
                return 0;
            }
        } else if (!(func_0011DF78(dy) <= 17.0f + (*(float **)(arg1 + 0x30))[1])) {
            return 0;
        }
        if (*(unsigned char *)(arg1 + 8) == 4) {
            ang = func_001B1470(*(float *)(arg0 + 0xC4) - *(float *)(arg1 + 0xC4));
        } else {
            sub = *(unsigned char *)(arg1 + 3);
            if (sub == 2 && (*(unsigned char *)(arg1 + 2) & 0xF) == 7) {
                if (func_0011DF78(*(float *)(arg1 + 0xC0)) > func_0011DF78(*(float *)(arg1 + 0xC8))) {
                    bmax = func_0011DF78(*(float *)(arg1 + 0xC0));
                } else {
                    bmax = func_0011DF78(*(float *)(arg1 + 0xC8));
                }
                if (bmax <= 0.7853982f) {
                    ang = func_001B1470((3.1415927f + *(float *)(arg0 + 0xC4)) - *(float *)(arg1 + 0xC4));
                } else if (bd <= 7.0f) {
                    ang = 0.0f;
                } else {
                    ang = func_001B1470(*(float *)(arg0 + 0xC4) - func_0011E620(bx, bz));
                }
            } else if (sub == 1) {
                ang = func_001B1470((3.1415927f + *(float *)(arg0 + 0xC4)) - *(float *)(arg1 + 0xC4));
            } else if (bd <= 7.0f) {
                ang = 0.0f;
            } else {
                ang = func_001B1470(*(float *)(arg0 + 0xC4) - func_0011E620(bx, bz));
            }
        }
        if (func_0011DF78(ang) <= 1.5707964f) {
            if ((*(unsigned char *)(arg1 + 2) & 0xF) == 7 ||
                *(int *)(arg1 + 0x10) == (int)func_00219550) {
                *(float *)0x70003600 = *(float *)(arg0 + 0xA0);
                *(float *)0x70003604 = 16.0f + *(float *)(arg0 + 0xA4);
                *(float *)0x70003608 = *(float *)(arg0 + 0xA8);
                *(int *)0x7000360C = 0x3F800000;
                if (func_0019A910(D_70003600, arg1 + 0xB0, 6) != 0 &&
                    (*(short *)(*(int *)0x700031D0 + 0x1A) & 0x2800) != 0) {
                    if (*(int *)0x700031D8 != 2 || *(int *)0x700031D4 != (int)arg1) {
                        return 0;
                    }
                }
            }
            return 1;
        }
        return 0;
    }

    case 5:
        dx = *(float *)(arg1 + 0xB0) - *(float *)(arg0 + 0xA0);
        dz = *(float *)(arg1 + 0xB8) - *(float *)(arg0 + 0xA8);
        if (!(dx * dx + dz * dz <= 196.0f)) {
            return 0;
        }
        if (func_0011DF78(*(float *)(arg1 + 0xB4) - *(float *)(arg0 + 0xA4)) > 4.0f) {
            return 0;
        }
        return 1;
    }

    if (func_0011DF78(ang) <= 0.7853982f) {
        return 1;
    }
    return 0;
}
