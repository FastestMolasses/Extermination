// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Five-state scripted-sequence tick for actor `self` driving the owner/anim
// context `ctx`, dispatched on the state byte at self+6.
//   state 0: advance state, arm the context (ctx+0x40 = 1.0 rate, ctx+0x80/0x50/
//            0x4C cleared) and kick clip 0x32.
//   state 1: wait for the owner input bit 0x1000 at ctx+0x20; on it, advance the
//            state, seed the countdown at ctx+0x28 from the RNG (0x3C..0x13B) and
//            kick clip 0x33 plus event 0x7EC.
//   state 2: count ctx+0x28 down; at zero advance the state, set the ctx+0x85
//            hold value, release the handler (func_001EFE00 0x8000002B) and kick
//            clip 0x34 plus event 0x7ED.
//   state 3: hold ctx+0x85; on the input bit, when the heading self+0x60 has
//            already reached 1.15 advance the state and kick clip 0x35 plus event
//            0x7EE.  Then step the heading vector toward the 0.001 scratchpad
//            delta at 0x700038A0 and clamp self+0x60..0x6C to {1.15,1.15,1.15,1.0}.
//   state 4: hold ctx+0x85; on the input bit clear self+5/self+6, set the "done"
//            flag bit 0x80 at self+0xD, load the exit timer at self+0x34 with
//            0x190 or 0xFA depending on the global D_0081070A, and fire 0x7EF.
// idiom-24 (`zi = 0; z = (float)zi;`) at the two anim_clip_init sites whose
// trailing 0.0f argument must be emitted as `mtc1 zero,$f13` BEFORE the
// `mtc1 $v0,$f12` that stages 5.0f; each site needs its own int/float pair.
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001028B8(float *a, float *b, float *c);
extern int func_00122BB8(void);
extern char *func_001EFE00(int a, char *b);
extern void func_001FBD50(char *self, int id, int b, float f);
extern unsigned char D_0081070A;
extern float D_700038A0;

void func_0014A350(char *self, char *ctx) {
    unsigned char st;
    int t;
    int zi0;
    float z0;
    int zi2;
    float z2;

    st = *(unsigned char *)(self + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(self + 6) = st + 1;
        *(float *)(ctx + 0x40) = 1.0f;
        *(char *)(ctx + 0x80) = 0;
        *(int *)(ctx + 0x50) = 0;
        *(int *)(ctx + 0x4C) = 0;
        zi0 = 0;
        z0 = (float)zi0;
        anim_clip_init(self, 0x32, 5.0f, z0);
        break;
    case 1:
        if (*(int *)(ctx + 0x20) & 0x1000) {
            *(unsigned char *)(self + 6) = st + 1;
            *(int *)(ctx + 0x28) = ((func_00122BB8() >> 11) & 0xFF) + 0x3C;
            anim_clip_init(self, 0x33, 0.0f, 0.0f);
            func_001FBD50(self, 0x7EC, 0, 300.0f);
        }
        break;
    case 2:
        t = *(int *)(ctx + 0x28);
        if (t != 0) {
            *(int *)(ctx + 0x28) = t - 1;
            break;
        }
        *(unsigned char *)(self + 6) = st + 1;
        *(char *)(ctx + 0x85) = 0x64;
        func_001EFE00(0x8000002B, self);
        zi2 = 0;
        z2 = (float)zi2;
        anim_clip_init(self, 0x34, 5.0f, z2);
        func_001FBD50(self, 0x7ED, 0, 300.0f);
        break;
    case 3:
        *(char *)(ctx + 0x85) = 0x64;
        if (*(int *)(ctx + 0x20) & 0x1000) {
            if (*(float *)(self + 0x60) == 1.15f) {
                *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
                anim_clip_init(self, 0x35, 0.0f, 0.0f);
                func_001FBD50(self, 0x7EE, 0, 300.0f);
            }
        }
        *(volatile float *)0x700038A0 = 0.001f;
        *(volatile float *)0x700038A4 = 0.001f;
        *(volatile float *)0x700038A8 = 0.001f;
        *(volatile int *)0x700038AC = 0;
        func_001028B8((float *)(self + 0x60), &D_700038A0, (float *)(self + 0x60));
        if (!(*(float *)(self + 0x60) <= 1.15f)) {
            *(float *)(self + 0x60) = 1.15f;
            *(float *)(self + 0x64) = 1.15f;
            *(float *)(self + 0x68) = 1.15f;
            *(float *)(self + 0x6C) = 1.0f;
        }
        break;
    case 4:
        *(char *)(ctx + 0x85) = 0x64;
        if (*(int *)(ctx + 0x20) & 0x1000) {
            *(char *)(self + 5) = 0;
            *(char *)(self + 6) = 0;
            *(unsigned char *)(self + 0xD) = *(unsigned char *)(self + 0xD) | 0x80;
            if (D_0081070A != 0) {
                *(short *)(self + 0x34) = 0x190;
            } else {
                *(short *)(self + 0x34) = 0xFA;
            }
            func_001FBD50(self, 0x7EF, 0, 300.0f);
        }
        break;
    }
}
