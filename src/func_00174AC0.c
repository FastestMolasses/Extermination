// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-entity aim/turn update for the gyro-driven aiming mode. If the
// scratchpad disable flag at 0x70003B8D is set, clears the aim state
// (+0x23F mode, +0x240 rate, +0x24C angle) and returns 0. Otherwise it
// latches the global aim mode D_00810E57 into +0x23F and picks the per-mode
// turn rate stored at +0x240 (0.1 / 0.3 / 0.8 as raw float bit patterns;
// mode 0 clears and returns).
//
// The two gyro bytes D_00810E64/D_00810E65 are converted with the
// (float)(unsigned int) idiom, scaled by pi/256, run through func_0011DE90
// (sin/cos helper) into +0x244/+0x248, combined by func_0011E620 (atan2)
// into +0x24C, and normalized by func_001B1470 (wrap to +-pi) to give the
// desired heading `ang`.
//
// If the entity is active (+5 == 1) and its action state (+0x1F0) is already
// 6 or 7 (turning), aiming is suppressed (arg1 = 0). Otherwise, when the
// entity is moving faster than 0.5 and the aim mode is >= 2, a heading error
// beyond +-3pi/4 forces action state 7 with sub-state 4 (turn right) or 3
// (turn left) and suppresses aiming.
//
// arg1 == 1 then smooths the facing angle at +0xC4 toward `ang` through
// func_001B12B0(target, current, max_step) with a rate chosen by aim mode
// when standing still, or by heading error and speed when moving; arg1 == 2
// only records `ang` into +0x218. Returns the latched aim mode.
//
// Matching keys: (1) the 6/7 suppression test must be written as
// `if (st == 7) goto zeroarg; if (st == 6) { zeroarg: arg1 = 0; } else {...}`
// - the goto-into-the-then-block is what makes mwcc emit the target's
// `beql st,7` (arg1=0 peeled into the delay slot, target +4) followed by an
// inverted `bne st,6` that falls through into the shared block. An if/else-if
// chain duplicates the block and its exit branch; `st == 7 || st == 6`
// collapses into a `sltiu` range test; a `switch` gets the beql but emits
// `beq` + `b default` instead of the inverted fall-through.
// (2) mwcc 2.3.3 is required: the pinned 991202 build caps at 96.86% and
// 2.4.1 at 99.47%.
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
        if (st == 7) goto zeroarg;
        if (st == 6) {
        zeroarg:
            arg1 = 0;
        } else {
            if (!(*(float *)(arg0 + 0x38) <= 0.5f) && (int)*(unsigned char *)(arg0 + 0x23F) >= 2) {
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
