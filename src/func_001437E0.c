// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Two-state hit-flash/attack-scale tick for actor `arg0` driving context `arg1`: state 0
// advances the sub-state, resets ctx timers (0x20/0x4C/0x50), seeds scale 0x8C = 1.0 and
// ramp-rate 0x54 = +0.08, flags ctx+0x72 = 0xFF, then registers tag 0x8000004F/0x8000004E
// and starts anim clip 0x25 (start frame 132.0 or 0.0) depending on ctx+0x78; state 1 ramps
// the scale by the rate while the actor isn't turning and its speed 0x3C <= 110.0, clamps at
// 4.0 (flipping the rate to -0.08, swapping the model/sound via D_0028A490[0x7D|0x7E] and
// queueing event 0x83D/0x83C once) or at 1.0 on the way down (zeroing the rate); finally,
// when ctx+0x30 bit 0x1000 is set and the rate has settled to 0, it resets the actor state
// bytes and reseeds the ctx+0x60 cooldown to a pseudo-random 300..1199.
extern void anim_clip_init(char *a0, int a1, float a2, float a3);
extern int func_00122BB8(void);
extern void func_001CA5E0(char *a0, int a1, int a2);
extern void func_001EFE00(int a0, char *a1, int a2);
extern void func_001FBD50(char *a0, int a1, int a2, float a3);

extern int D_0028A490[];

void func_001437E0(char *arg0, char *arg1) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 0x20) = 0;
        *(float *)(arg0 + 0x8C) = 1.0f;
        *(float *)(arg1 + 0x54) = 0.08f;
        *(int *)(arg1 + 0x4C) = 0;
        *(int *)(arg1 + 0x50) = 0;
        *(unsigned char *)(arg1 + 0x72) = 0xFF;

        if (*(signed char *)(arg1 + 0x78) != 0) {
            func_001EFE00(0x8000004F, arg0, 1);
            anim_clip_init(arg0, 0x25, 10.0f, 132.0f);
        } else {
            int zi = 0;
            float z = (float)zi;
            func_001EFE00(0x8000004E, arg0, 1);
            anim_clip_init(arg0, 0x25, 10.0f, z);
        }
        break;

    case 1:
        *(unsigned char *)(arg1 + 0x72) = 0xFF;

        if (!(*(short *)(arg0 + 0x2C) & 0x8000) &&
            *(float *)(arg0 + 0x3C) <= 110.0f) {
            if (*(float *)(arg1 + 0x54) != 0.0f) {
                *(float *)(arg0 + 0x8C) = *(float *)(arg0 + 0x8C) + *(float *)(arg1 + 0x54);

                if (*(float *)(arg1 + 0x54) > 0.0f) {
                    if (!(*(float *)(arg0 + 0x8C) < 4.0f)) {
                        int idx;

                        *(float *)(arg0 + 0x8C) = 4.0f;
                        *(float *)(arg1 + 0x54) = -0.08f;
                        idx = (*(unsigned char *)(arg0 + 0xD) & 0x80) ? 0x7E : 0x7D;

                        if (*(signed char *)(arg1 + 0x78) != 0) {
                            *(signed char *)(arg1 + 0x78) = 0;
                            func_001CA5E0(arg0, D_0028A490[idx], 0);
                            *(int *)(arg0 + 0x80) = 0x3F800000;
                            *(int *)(arg0 + 0x84) = 0x3F800000;
                            *(int *)(arg0 + 0x88) = 0x3F800000;

                            if (*(int *)(arg1 + 0x20) == 0) {
                                *(int *)(arg1 + 0x20) = 1;
                                func_001FBD50(arg0, 0x83D, 0, 300.0f);
                            }
                        } else {
                            *(signed char *)(arg1 + 0x78) = 1;
                            func_001CA5E0(arg0, D_0028A490[idx], 8);
                            *(int *)(arg0 + 0x80) = 0x3F5C0000;
                            *(int *)(arg0 + 0x84) = 0x3F616666;
                            *(int *)(arg0 + 0x88) = 0x3F616666;
                            *(float *)(arg0 + 0x8C) = 3.5f;

                            if (*(int *)(arg1 + 0x20) == 0) {
                                *(int *)(arg1 + 0x20) = 1;
                                func_001FBD50(arg0, 0x83C, 0, 300.0f);
                            }
                        }
                    }
                } else if (*(float *)(arg0 + 0x8C) <= 1.0f) {
                    *(float *)(arg0 + 0x8C) = 1.0f;
                    *(float *)(arg1 + 0x54) = 0.0f;
                }
            }
        }

        if ((*(int *)(arg1 + 0x30) & 0x1000) && *(float *)(arg1 + 0x54) == 0.0f) {
            *(signed char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(signed char *)(arg1 + 0x72) = 0;
            *(short *)(arg1 + 0x60) = (short)((func_00122BB8() >> 17) % 900 + 300);
        }
        break;
    }
}
