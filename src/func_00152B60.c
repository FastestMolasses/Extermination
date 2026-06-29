// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state init driver dispatched on the state byte at arg0+6 (arg0 = entity,
// arg1 = sibling/owner state block).
// State 0: advance state, set arg0+0=2, zero the 0xC0/0xC4/0xC8 vector and seed
//   the 0xB0..0xCC float block (100.0f/35.0f/95.0f/1.0f and two 1.0f), then
//   anim_clip_init(arg0, 0x18, 0,0).
// State 1: when arg1+0 has bit 0x1000, advance state, clear arg1+4, and run
//   anim_clip_init(arg0, 0x19, 0,0).
// State 2: if arg1+4==0 and the float at arg0+0x3C <= 150.0f, latch arg1+4=1 and
//   func_001FBD50(arg0, 0x88D, 0, 500.0f); then on bit 0x1000 fully re-arm
//   (arg0+0=1, arg0+5=0, state=0, arg1+0x42=0x258) and clear arg0->0x20 child +5.
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001FBD50(char *self, int a, int b, float f);

void func_00152B60(char *arg0, char *arg1) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(char *)(arg0 + 0) = 2;
        *(int *)(arg0 + 0xC8) = 0;
        *(int *)(arg0 + 0xC4) = 0;
        *(int *)(arg0 + 0xC0) = 0;
        *(int *)(arg0 + 0xCC) = 0x3F800000;
        *(int *)(arg0 + 0xB0) = 0x42C80000;
        *(int *)(arg0 + 0xB4) = 0x420C0000;
        *(int *)(arg0 + 0xB8) = 0x42BE0000;
        *(int *)(arg0 + 0xBC) = 0x3F800000;
        anim_clip_init(arg0, 0x18, 0.0f, 0.0f);
        break;
    case 1:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(int *)(arg1 + 4) = 0;
            anim_clip_init(arg0, 0x19, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (*(int *)(arg1 + 4) == 0) {
            if (*(float *)(arg0 + 0x3C) <= 150.0f) {
                *(int *)(arg1 + 4) = 1;
                func_001FBD50(arg0, 0x88D, 0, 500.0f);
            }
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            char *p;
            *(char *)(arg0 + 0) = 1;
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(short *)(arg1 + 0x42) = 0x258;
            p = *(char **)(arg0 + 0x20);
            if (p != 0) {
                *(char *)(p + 5) = 0;
            }
        }
        break;
    }
}
