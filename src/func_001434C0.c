// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Anim-state stepper. Switches on the state byte arg0[6]:
//  case 0: advance state, zero arg1 fields (0x20/0x75/0x4C/0x50), start clip 7.
//  case 1: if arg1[0x20]==0 and arg0.f32@0x3C <= 25.0, advance state, set
//          arg1[0x20]=1, seed arg1 floats @0x4C/@0x48 from globals D_002753E0/E4,
//          and kick func_001FBD50(self, 0x831, 0, 300.0f).
//  case 2: if arg1.flags@0x30 has bit 0x1000, start clip 8; then if
//          arg1.f32@0x48 <= 0.0, set arg0[5]=4, reset state, start clip 9.
// The case-1 store of 1 to arg1[0x20] reuses the constant from the ==1 dispatch
// test (matters for the match). D_002753E0/E4 are gp-relative -> sdatathreshold 4.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): under the pinned 991202 the
// residual is the float-compare (bc1f / c.le.s) delay-slot-fill wall, stalling
// at 93.92%. 2.3.3 leaves those delay-slot nops -> objdiff 100%.
extern void anim_clip_init(void *self, int clip, float f12, float f13);
extern void func_001FBD50(void *self, int a1, int a2, float f12);
extern float D_002753E0;
extern float D_002753E4;

void func_001434C0(unsigned char *arg0, char *arg1) {
    unsigned char state;

    state = arg0[6];
    switch (state) {
    case 0:
        arg0[6] = state + 1;
        *(int *)(arg1 + 0x20) = 0;
        arg1[0x75] = 0;
        *(int *)(arg1 + 0x4C) = 0;
        *(int *)(arg1 + 0x50) = 0;
        anim_clip_init(arg0, 7, 1.0f, 0.0f);
        break;
    case 1:
        if (*(int *)(arg1 + 0x20) == 0 && *(float *)(arg0 + 0x3C) <= 25.0f) {
            arg0[6] = state + 1;
            *(int *)(arg1 + 0x20) = 1;
            *(float *)(arg1 + 0x4C) = D_002753E0;
            *(float *)(arg1 + 0x48) = D_002753E4;
            func_001FBD50(arg0, 0x831, 0, 300.0f);
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            anim_clip_init(arg0, 8, 1.0f, 0.0f);
        }
        if (*(float *)(arg1 + 0x48) <= 0.0f) {
            arg0[5] = 4;
            arg0[6] = 0;
            anim_clip_init(arg0, 9, 1.0f, 0.0f);
        }
        break;
    }
}
