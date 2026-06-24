// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Two-phase actor tick keyed on the state byte arg0[6] (arg0 = actor/self,
// arg1 = controller/owner state block). Any state other than 0 or 1 returns.
//   State 0 (first entry): bump arg0[6] to 1, mask the owner flag byte
//     arg1[0x8D] &= 1 (signed-char field), clear arg1[0x4C], and start anim
//     clip 6 via anim_clip_init(self, 6, 1.0f, 0.0f). Then FALL THROUGH into
//     the state-1 body.
//   State 1 (running, also reached by fall-through): if the owner flag word
//     arg1[0x20] has bit 0x1000 set, finish the move: arg0[5]=4, arg0[6]=0,
//     seed arg1[0x4C]=0.4f (0x3ECCCCCD) and arg1[0x48]=2.1f (0x40066666),
//     kick func_001FBD50(self, 0x7E6, 0, 300.0f). If additionally the signed
//     flag byte arg1[0x8D] == 1, build a scratch transform in D_700038A0 from
//     the actor launch point (self+0xB0): func_00102948(D_700038A0, self+0xB0),
//     copy arg1[0x6C] into D_700038A0+4, then func_001EFD90(0x80000016,
//     D_700038A0, self+0xC0) and func_001E8B90(D_700038A0, 5.0f).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the
// clean-store conditional-branch delay-slot nops match under 2.3.3; 991202
// fills those slots and stalls at 92.8%. Verified objdiff 100.0 (70/70
// instructions) vs build/expected/func_00148400.o.
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001FBD50(int self, int a, int b, float c);
extern void func_00102948(void *dst, void *src);
extern void func_001EFD90(int id, void *a, void *b);
extern void func_001E8B90(void *p, float f);
extern char D_700038A0[];

void func_00148400(unsigned char *arg0, unsigned char *arg1) {
    switch (arg0[6]) {
    case 0:
        arg0[6] = arg0[6] + 1;
        *(signed char *)(arg1 + 0x8D) = *(signed char *)(arg1 + 0x8D) & 1;
        *(int *)(arg1 + 0x4C) = 0;
        anim_clip_init((int)arg0, 6, 1.0f, 0.0f);
        /* fallthrough */
    case 1:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            arg0[5] = 4;
            arg0[6] = 0;
            *(int *)(arg1 + 0x4C) = 0x3ECCCCCD;
            *(int *)(arg1 + 0x48) = 0x40066666;
            func_001FBD50((int)arg0, 0x7E6, 0, 300.0f);
            if (*(signed char *)(arg1 + 0x8D) == 1) {
                func_00102948(D_700038A0, arg0 + 0xB0);
                *(float *)0x700038A4 = *(float *)(arg1 + 0x6C);
                func_001EFD90(0x80000016, D_700038A0, arg0 + 0xC0);
                func_001E8B90(D_700038A0, 5.0f);
            }
        }
        break;
    }
}
