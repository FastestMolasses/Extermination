// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Two-state ambient anim/idle tick for actor `self` driving anim-context `ctx`:
// state 0 arms the idle timer and kicks the idle clip; state 1 counts the timer
// down, re-seeds it from the RNG on expiry, eases the current heading toward the
// target, then picks the next actor state (3 / 1 / 2) from range, a global flag
// and an RNG bit.  Afterwards, when the sfx cooldown at ctx+0x5E has run out, it
// re-seeds the cooldown and plays a random idle sound from D_00245CF0[0..3].
// The `zi = 0; z = (float)zi;` staging is load-bearing: it is what makes mwcc
// schedule `mtc1 zero,$f13` ahead of `mtc1 $v0,$f12` at the anim_clip_init call.
extern void anim_clip_init(char *self, int clip, float rate, float start);
extern int func_00122BB8(void);
extern float func_001B12B0(float target, float cur, float step);
extern void func_001FBD50(char *self, int id, int b, float f);
extern int D_00245CF0;
extern unsigned char D_008104E4;

void func_0014D5F0(char *self, char *ctx) {
    int t;
    int st;
    int zi;
    float z;

    st = *(unsigned char *)(self + 6);
    switch (st) {
    case 0:
        *(char *)(self + 6) = st + 1;
        *(int *)(ctx + 4) = 0;
        *(float *)(ctx + 0x24) = *(float *)(ctx + 0x38);
        *(char *)(ctx + 0x5D) = 0;
        if (*(int *)(self + 0x20) != 0) {
            *(char *)(*(int *)(self + 0x20) + 5) = 0;
        }
        if ((*(short *)(self + 0x2C) & 0xFFFF7FFF) != 2) {
            zi = 0;
            z = (float)zi;
            anim_clip_init(self, 2, 5.0f, z);
        }
        break;
    case 1:
        t = *(int *)(ctx + 4);
        if (t != 0) {
            *(int *)(ctx + 4) = t - 1;
        } else {
            *(int *)(ctx + 4) = (func_00122BB8() >> 14) & 0x7F;
            *(float *)(ctx + 0x24) = *(float *)(ctx + 0x38);
        }
        *(float *)(ctx + 0x28) = func_001B12B0(*(float *)(ctx + 0x24), *(float *)(ctx + 0x28), 0.04363323375582695f);
        if (*(short *)(ctx + 0x50) == 0) {
            if (*(float *)(ctx + 0x34) <= 25.0f && D_008104E4 != 1) {
                *(char *)(self + 5) = 3;
            } else {
                if ((func_00122BB8() >> 7) & 1) {
                    *(char *)(self + 5) = 1;
                } else {
                    *(char *)(self + 5) = 2;
                }
            }
            *(char *)(self + 6) = 0;
        }
        break;
    }
    if (*(char *)(ctx + 0x5E) == 0) {
        *(char *)(ctx + 0x5E) = ((func_00122BB8() >> 11) & 0x3F) + 0x3C;
        func_001FBD50(self, *(int *)((char *)&D_00245CF0 + (((func_00122BB8() >> 7) & 3) * 4)), 0, 300.0f);
    }
}
