// NEARMISS func_001437E0  (vram 0x001437E0, 0x304 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.11% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// dead-instruction-padding + fp-register-coloring: body/structure fully recovered (2-state animation state machine: case 0 inits attack timers/anim and picks a variant via anim_clip_init depending on arg1+0x78; case 1 ramps a scale field 0x8C by rate arg1+0x54, clamps at 4.0 (triggers hit sound via...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Animation/attack-scale state machine keyed on arg0+6 (a local actor sub-state):
// case 0 (init): advances the sub-state, resets arg1's flags/timers (0x20,0x4C,0x50),
// sets arg0+0x8C scale=1.0 and arg1+0x54 ramp-rate=0.08, flags arg1+0x72=0xFF, and
// picks one of two anim_clip_init variants (id 0x25) based on arg1+0x78, tagging
// via func_001EFE00(0x8000004F or 0x8000004E, arg0, 1).
// case 1 (update): flags arg1+0x72=0xFF; if arg0 isn't already turning
// (short at +0x2C bit 0x8000 clear) and its speed field +0x3C <= 110.0, and the
// ramp rate is nonzero, advances arg0+0x8C by the rate. If rate>0 and the new
// scale reaches >=4.0, clamps to 4.0, flips rate to -0.08, and depending on
// arg1+0x78 either triggers a "hit" sound via func_001CA5E0 + resets arg0's RGB
// scale (0x80/84/88) to 1.0 and queues effect 0x83D via func_001FBD50, or
// triggers an "impact" sound + sets RGB to a dimmer palette + scale=3.5 and
// queues effect 0x83C. If rate<=0 and the scale drops to <=1.0, clamps to 1.0
// and zeroes the rate. Finally, if arg1's flag word (+0x30) has bit 0x1000 set
// and the rate is exactly 0, resets arg0's state (+5,+6) and arg1+0x72, and
// reseeds arg1+0x60 with a pseudo-random 300..1199 range derived from
// func_00122BB8().

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
            return;
        }
        {
            float z;
            func_001EFE00(0x8000004E, arg0, 1);
            anim_clip_init(arg0, 0x25, 10.0f, (z = 0.0f));
        }
        return;

    case 1:
        *(unsigned char *)(arg1 + 0x72) = 0xFF;

        if (!(*(short *)(arg0 + 0x2C) & 0x8000) &&
            *(float *)(arg0 + 0x3C) <= 110.0f) {
            if (*(float *)(arg1 + 0x54) != 0.0f) {
                float newval = *(float *)(arg0 + 0x8C) + *(float *)(arg1 + 0x54);
                *(float *)(arg0 + 0x8C) = newval;

                if (*(float *)(arg1 + 0x54) > 0.0f) {
                    if (!(newval < 4.0f)) {
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
                } else if (newval <= 1.0f) {
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
        return;
    }
}
