// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state animation slice dispatched on the state byte at arg0+6 (switch
// 0/1/2/3). State 0: bumps state, zeroes the three counters at arg1+4/8/0xC,
// sets the 2.0f field at arg1+0x28 and the byte arg1+0x53 = 2, kicks clip 0x12
// via anim_clip_init(self, 0x12, 5.0f, 0.0f), then clears the +5 byte of the
// linked object at arg0+0x20 if present. State 1: when the +0x1000 event bit at
// arg1+0 is set, advances state and starts clip 0x13. State 2 (gated on the
// same +0x1000 bit): one-shots func_001FBD50(self, 0x88C, 0, 500.0f) the first
// time (arg1+0xC latch); runs a per-tick countdown at arg1+4 -- when it hits 0,
// reseeds it from (rng>>3)&0xF, builds a randomized transform in the scratch
// vec at 0x700038A0.. (x = 5*(b/255)-2.5, z = 57+5*(b/255)), applies it via
// func_001B2B10/func_001028B8/func_001283D0, fires func_001FBD50(self, 0x888,
// 0, 300.0f), and after 5 iterations (arg1+8) advances to clip 0x14. State 3:
// on the +0x1000 bit, resets state to 0, sets arg1+0x28 = 1.0f and re-arms the
// short timer at arg1+0x42 = ((rng>>7)&0x1FF)+0x258.
//
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// reaches 87.3%. Keys: (1) the dispatch is a `switch` on the local state byte so
// 2.3.3 emits the target's descending 3/2/1/0 compare chain with the case-0
// branch-likely (beql) carrying the `addiu v0,st,1` advance in its delay slot;
// (2) func_00122BB8 is signed int (the >>3/>>0x12/>>0xD are arithmetic shifts
// and the (float) casts of the &0xFF results are plain cvt.s.w, no unsigned
// fixup); (3) the 0x700038A0.. scratch is written via raw absolute addresses
// (the stores) but passed to calls as &D_700038A0 (%hi/%lo); (4) func_001B2B10
// takes only 3 args (no trailing float), and the first func_001FBD50 reuses the
// self pointer already live in $a0.
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001028B8(float *a, char *b, float *c);
extern int func_00122BB8(void);
extern void func_001283D0(float *a, int b, char *c);
extern void func_001B2B10(char *self, float *a, float *b);
extern void func_001FBD50(char *p, int a, int b, float f);

extern float D_700038A0;
extern float D_700038A4;
extern float D_700038A8;
extern float D_700038AC;

void func_00152660(char *arg0, char *arg1) {
    unsigned char st;
    int n;
    char *p;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 4) = 0;
        *(int *)(arg1 + 8) = 0;
        *(int *)(arg1 + 0xC) = 0;
        *(float *)(arg1 + 0x28) = 2.0f;
        *(char *)(arg1 + 0x53) = 2;
        anim_clip_init((int)arg0, 0x12, 5.0f, 0.0f);
        p = *(char **)(arg0 + 0x20);
        if (p != 0) {
            *(char *)(p + 5) = 0;
        }
        break;
    case 1:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            anim_clip_init((int)arg0, 0x13, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0) & 0x1000) {
            if (*(int *)(arg1 + 0xC) == 0) {
                *(int *)(arg1 + 0xC) = 1;
                func_001FBD50(arg0, 0x88C, 0, 500.0f);
            }
            n = *(int *)(arg1 + 4);
            if (n != 0) {
                *(int *)(arg1 + 4) = n - 1;
            } else {
                *(int *)(arg1 + 4) = (func_00122BB8() >> 3) & 0xF;
                *(float *)0x700038A8 = 0.0f;
                *(int *)0x700038A4 = 0;
                *(float *)0x700038A0 = 0.0f;
                *(float *)0x700038AC = 1.0f;
                *(float *)0x700038A4 = 17.0f;
                *(float *)0x700038A0 = (5.0f * ((float)((func_00122BB8() >> 0x12) & 0xFF) / 255.0f)) - 2.5f;
                *(float *)0x700038A8 = 57.0f + (5.0f * ((float)((func_00122BB8() >> 0xD) & 0xFF) / 255.0f));
                func_001B2B10(arg0, &D_700038A0, &D_700038A0);
                func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);
                func_001283D0(&D_700038A0, 1, arg0 + 0xC0);
                func_001FBD50(arg0, 0x888, 0, 300.0f);
                n = *(int *)(arg1 + 8) + 1;
                *(int *)(arg1 + 8) = n;
                if (n >= 5) {
                    *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                    anim_clip_init((int)arg0, 0x14, 0.0f, 0.0f);
                }
            }
        }
        break;
    case 3:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(float *)(arg1 + 0x28) = 1.0f;
            *(short *)(arg1 + 0x42) = ((func_00122BB8() >> 7) & 0x1FF) + 0x258;
        }
        break;
    }
}
