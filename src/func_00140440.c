// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Four-phase actor state-machine tick keyed on the state byte self[6]
// (switch 0..3). Phase 0: bump state, init anim fields at arg1+0x40..0x50,
// kick clip 0x26 via anim_clip_init(self, 0x26, 5.0f, 0.0f). Phases 1/2:
// gated on the actor flag word arg1+0x30 bit 0x1000; advance state, start
// clips 0x27/0x28 and fire func_001FBD50 sound/event ids 0x812/0x813;
// phase 2 also sets arg1+0x88 = 100 and calls func_001EFE00(0x8000002B,...).
// Phase 3: set arg1+0x88 = 100; if flag 0x1000 set and self+0x60 == 1.1f,
// finish (self[5]=4, self[6]=0), set self[0xD] |= 0x80, choose self+0x34 =
// 200 or 100 based on D_0081070A, re-init bone defaults and fire id 0x815.
// Always writes the scratch vec at 0x700038A0..AC (0.001f x3, 0) and calls
// func_001028B8, then clamps self+0x60 up to 1.1f (filling +0x64/68/6C).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the
// state-byte switch dispatch + conditional-branch delay-slot nops match
// under 2.3.3 (991202 only 83.6%). Verified objdiff 100.0 vs
// build/expected/func_00140440.o.
extern void anim_clip_init(int self, int clip, float a, float b);
extern void bone_init_default_2(int self, int a, int b);
extern void func_001028B8(int a, int *b, int c);
extern void func_001EFE00(int a, int self, int b);
extern void func_001FBD50(int self, int a, int b, float c);
extern unsigned char D_0081070A;
extern int D_700038A0;

void func_00140440(unsigned char *self, unsigned char *arg1) {
    unsigned char st = self[6];
    switch (st) {
    case 0:
        self[6] = st + 1;
        arg1[0x81] = 0;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        anim_clip_init((int)self, 0x26, 5.0f, 0.0f);
        break;
    case 1:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            self[6] = st + 1;
            anim_clip_init((int)self, 0x27, 5.0f, 0.0f);
            func_001FBD50((int)self, 0x812, 0, 300.0f);
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            self[6] = st + 1;
            arg1[0x88] = 0x64;
            func_001EFE00(0x8000002B, (int)self, st);
            anim_clip_init((int)self, 0x28, 5.0f, 0.0f);
            func_001FBD50((int)self, 0x813, 0, 300.0f);
        }
        break;
    case 3:
        arg1[0x88] = 0x64;
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            if (*(float *)(self + 0x60) == 1.1f) {
                self[5] = 4;
                self[6] = 0;
                self[0xD] = self[0xD] | 0x80;
                if (D_0081070A != 0) {
                    *(short *)(self + 0x34) = 0xC8;
                } else {
                    *(short *)(self + 0x34) = 0x64;
                }
                bone_init_default_2((int)self, 0, st);
                func_001FBD50((int)self, 0x815, 0, 300.0f);
            }
        }
        *(int *)0x700038A0 = 0x3A83126F;
        *(int *)0x700038A4 = 0x3A83126F;
        *(int *)0x700038A8 = 0x3A83126F;
        *(int *)0x700038AC = 0;
        func_001028B8((int)(self + 0x60), &D_700038A0, (int)(self + 0x60));
        if (!(*(float *)(self + 0x60) <= 1.1f)) {
            *(float *)(self + 0x60) = 1.1f;
            *(int *)(self + 0x64) = 0x3F8CCCCD;
            *(int *)(self + 0x68) = 0x3F8CCCCD;
            *(int *)(self + 0x6C) = 0x3F800000;
        }
        break;
    }
}
