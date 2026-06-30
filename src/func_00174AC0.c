// NEARMISS func_00174AC0  (vram 0x00174AC0, 0x508 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.48% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// branch-likely-sense: st==7 dispatch emits plain `bne` vs target `beql` (idiom-20 likely-branch with arg1=0 store in delay slot). Body otherwise byte-identical; rest of diff is pure one-instruction offset cascade. Permuter/likely-branch territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 98.48% (mwcc 2.3.3). Body/logic fully recovered; sole residual is a
// branch-likely-sense artifact: the st==7 dispatch compiles to a plain `bne`
// where the target emits `beql` (idiom-20 likely branch with the arg1=0 store
// in its delay slot). Everything past that point is byte-identical except a
// one-instruction offset cascade. mwcc 2.3.3 scores higher than 991202 (95.05%).
// Per-aim ECM/turret aim-state update: state in *(u8*)(p+0x23F) from D_00810E57,
// computes target heading from gyro bytes D_00810E64/E65, smooths *(f32*)(p+0xC4)
// toward it via func_001B12B0 with state/speed-dependent rates.

extern float func_0011DE90(float);
extern float func_0011DF78(float);
extern float func_0011E620(float, float);
extern float func_001B12B0(float, float, float);
extern float func_001B1470(float);
extern float D_008106A0;
extern unsigned char D_00810E57;
extern unsigned char D_00810E64;
extern unsigned char D_00810E65;

unsigned char func_00174AC0(char *arg0, int arg1) {
    int st;
    float ang;
    float f64, f65;
    float tmp;

    if (*(unsigned char *)0x70003B8D != 0) {
        *(unsigned char *)(arg0 + 0x23F) = 0;
        *(int *)(arg0 + 0x240) = 0;
        *(float *)(arg0 + 0x24C) = 0.0f;
        return 0;
    }
    *(unsigned char *)(arg0 + 0x23F) = D_00810E57;
    switch (*(unsigned char *)(arg0 + 0x23F)) {
    case 0:
        *(int *)(arg0 + 0x240) = 0;
        *(float *)(arg0 + 0x24C) = 0.0f;
        return 0;
    case 1:
        *(int *)(arg0 + 0x240) = 0x3DCCCCCD;
        break;
    case 2:
        *(int *)(arg0 + 0x240) = 0x3E99999A;
        break;
    case 3:
        *(int *)(arg0 + 0x240) = 0x3F4CCCCD;
        break;
    }

    f65 = 3.1415927f * ((float)(unsigned int)D_00810E65 / 256.0f);
    f64 = (float)(unsigned int)D_00810E64;
    *(float *)(arg0 + 0x244) = func_0011DE90(3.1415927f * (f64 / 256.0f));
    *(float *)(arg0 + 0x248) = func_0011DE90(f65);
    *(float *)(arg0 + 0x24C) = func_0011E620(-*(float *)(arg0 + 0x248), *(float *)(arg0 + 0x244));
    ang = func_001B1470(3.1415927f + *(float *)(arg0 + 0x24C) + D_008106A0);

    if (*(unsigned char *)(arg0 + 5) == 1) {
        st = *(unsigned char *)(arg0 + 0x1F0);
        if (st == 7) {
            arg1 = 0;
        } else if (st == 6) {
            arg1 = 0;
        } else if (!(*(float *)(arg0 + 0x38) <= 0.5f) && (int)*(unsigned char *)(arg0 + 0x23F) >= 2) {
            tmp = func_001B1470(ang - *(float *)(arg0 + 0xC4));
            *(float *)0x70003A20 = tmp;
            if (!(tmp <= 2.3561945f)) {
                *(unsigned char *)(arg0 + 0x1F0) = 7;
                *(char *)(arg0 + 0x1F1) = 4;
                arg1 = 0;
            } else if (tmp < -2.3561945f) {
                *(unsigned char *)(arg0 + 0x1F0) = 7;
                *(char *)(arg0 + 0x1F1) = 3;
                arg1 = 0;
            }
        }
    }

    if (arg1 == 1) {
        if (*(float *)(arg0 + 0x38) == 0.0f) {
            st = *(unsigned char *)(arg0 + 0x23F);
            if (st == 1) {
                *(float *)(arg0 + 0xC4) = func_001B12B0(ang, *(float *)(arg0 + 0xC4), 0.06981317f);
            } else if (st == 2) {
                *(float *)(arg0 + 0xC4) = func_001B12B0(ang, *(float *)(arg0 + 0xC4), 0.13962634f);
            } else {
                *(float *)(arg0 + 0xC4) = func_001B12B0(ang, *(float *)(arg0 + 0xC4), 0.39269909f);
            }
            if ((int)*(unsigned char *)(arg0 + 0x23F) < 2 && ang != *(float *)(arg0 + 0xC4)) {
                *(char *)(arg0 + 0x25D) = 1;
            }
        } else {
            tmp = func_001B1470(ang - *(float *)(arg0 + 0xC4));
            *(float *)0x70003A20 = tmp;
            tmp = func_0011DF78(tmp);
            *(float *)0x70003A20 = tmp;
            if (!(tmp <= 0.9424779f)) {
                if (*(float *)(arg0 + 0x38) <= 0.1f) {
                    *(float *)(arg0 + 0xC4) = func_001B12B0(ang, *(float *)(arg0 + 0xC4), 0.10471976f);
                } else if (*(float *)(arg0 + 0x38) <= 0.3f) {
                    *(float *)(arg0 + 0xC4) = func_001B12B0(ang, *(float *)(arg0 + 0xC4), 0.15707964f);
                } else {
                    *(float *)(arg0 + 0xC4) = func_001B12B0(ang, *(float *)(arg0 + 0xC4), 0.18325958f);
                }
            } else {
                if (*(float *)(arg0 + 0x38) <= 0.1f) {
                    *(float *)(arg0 + 0xC4) = func_001B12B0(ang, *(float *)(arg0 + 0xC4), 0.06981317f);
                } else if (*(float *)(arg0 + 0x38) <= 0.3f) {
                    *(float *)(arg0 + 0xC4) = func_001B12B0(ang, *(float *)(arg0 + 0xC4), 0.10471976f);
                } else {
                    *(float *)(arg0 + 0xC4) = func_001B12B0(ang, *(float *)(arg0 + 0xC4), 0.122173056f);
                }
            }
        }
    } else if (arg1 == 2) {
        *(float *)(arg0 + 0x218) = ang;
    }
    return *(unsigned char *)(arg0 + 0x23F);
}
