// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Player/actor fall + forward-slide step. Bails if bit0 of state byte 0x75 is
// set. Otherwise decays the vertical velocity at ctl+0x48 by 0.1 and folds it
// into the actor's Y at +0xB4. If the actor is in the "bit1 set, bit7 clear"
// state, is below the 18.0 height at ctl+0x5C, and func_001B1560 reports a hit
// against D_00810360 at ~65 degrees (1.13446f), it raises flag bit0 in the
// halfword at +0x52 and stops. Otherwise it accumulates the horizontal speed
// (ctl+0x4C += ctl+0x50) and advances X/Z at +0xB0/+0xB8 by speed * cos/sin of
// the heading at +0xC4.

extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern int func_001B1560(void *a, void *b, float f);
extern int D_00810360;

void func_00145880(char *arg0, char *arg1) {
    float v;
    int f;

    if (*(char *)(arg1 + 0x75) & 1) {
        return;
    }
    v = *(float *)(arg1 + 0x48);
    v += -0.1f;
    *(float *)(arg1 + 0x48) = v;
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + v;
    f = *(char *)(arg1 + 0x75);
    if ((f & 2) && !(f & 0x80) && *(float *)(arg1 + 0x5C) <= 18.0f &&
        func_001B1560(arg0, &D_00810360, 1.1344640254974365f) != 0) {
        *(unsigned short *)(arg0 + 0x52) = *(unsigned short *)(arg0 + 0x52) | 1;
        return;
    }
    *(float *)(arg1 + 0x4C) += *(float *)(arg1 + 0x50);
    *(float *)(arg0 + 0xB0) =
        *(float *)(arg0 + 0xB0) + *(float *)(arg1 + 0x4C) * func_0011E2A8(*(float *)(arg0 + 0xC4));
    *(float *)(arg0 + 0xB8) =
        *(float *)(arg0 + 0xB8) + *(float *)(arg1 + 0x4C) * func_0011DE90(*(float *)(arg0 + 0xC4));
}
