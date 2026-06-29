// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Entity animation state machine on the state byte e[6] (0..4). State 0 kicks
// off anim clip 0x70 and sets a ready flag; state 1 waits for input bit 0x1000;
// state 2 starts clip 0x71 and seeds a 12-step interpolation of three floats
// from the current pose (e+0xB0..) toward the target pose (e+0x290..) storing
// per-step deltas at e+0x2E0..; state 3 advances the interpolation each tick
// (or snaps to target + plays clip 0x7A / sound 0xFE when the counter expires);
// state 4 waits for input bit 0x1000 then resets to state 0 and plays clip 0x7B.
extern int func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern void func_001FBD50(unsigned char *e, int id, int b, float f);

void func_001645D0(unsigned char *e) {
    int st;
    short t;

    st = e[6];
    switch (st) {
    case 0:
        e[6] = st + 1;
        e[7] = 0;
        func_001749A0(e, 0x70, 0, 1.0f);
        e[0x25F] = 1;
        break;
    case 1:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
        }
        break;
    case 2:
        func_001749A0(e, 0x71, 0, 1.0f);
        e[6] = e[6] + 1;
        *(short *)(e + 0x28) = 0xC;
        *(float *)(e + 0x2E0) = (*(float *)(e + 0x290) - *(float *)(e + 0xB0)) / 12.0f;
        *(float *)(e + 0x2E4) = (*(float *)(e + 0x294) - *(float *)(e + 0xB4)) / 12.0f;
        *(float *)(e + 0x2E8) = (*(float *)(e + 0x298) - *(float *)(e + 0xB8)) / 12.0f;
        break;
    case 3:
        t = *(short *)(e + 0x28);
        *(short *)(e + 0x28) = t - 1;
        if (t == 0) {
            e[6] = e[6] + 1;
            *(float *)(e + 0xB0) = *(float *)(e + 0x290);
            *(float *)(e + 0xB4) = *(float *)(e + 0x294);
            *(float *)(e + 0xB8) = *(float *)(e + 0x298);
            func_001749A0(e, 0x7A, 0, 1.0f);
            func_001FBD50(e, 0xFE, 0, 300.0f);
        } else {
            *(float *)(e + 0xB0) += *(float *)(e + 0x2E0);
            *(float *)(e + 0xB4) += *(float *)(e + 0x2E4);
            *(float *)(e + 0xB8) += *(float *)(e + 0x2E8);
        }
        break;
    case 4:
        if (*(int *)(e + 0x200) & 0x1000) {
            func_001749A0(e, 0x7B, 0, 1.0f);
            e[5] = 9;
            e[6] = 0;
            e[0x1F0] = 0x10;
            e[0xD] = 1;
        }
        break;
    }
}
