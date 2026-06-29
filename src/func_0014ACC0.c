// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state behavior driver dispatched on the state byte at arg0+6 (3-way switch
// 0/1/2). arg0 = self entity, arg1 = a linked controller/physics block.
// State 0: bumps state, sets arg1+0x40 = 1.0f, zeroes arg1+0x80/+0x88 (bytes) and
//   arg1+0x50/+0x4C (words), kicks func_001B1190(*(u8*)(arg0+0x9A)), fires
//   func_001FBD50(self, 0x7EB, 0, 300.0f), then starts clip 0x2F (if arg1+0x87 set)
//   or 0x31 via anim_clip_init(self, clip, 1.0f, 0.0f).
// State 1: if (lh(arg0+0x2C) & 0xFFFF7FFF)==0x31 and arg0+0x3C == 54.0f, fires
//   func_001FBD50(self, 0x7E8, 0, 300.0f); then once arg1+0x20 has bit 0x1000 set,
//   advances state, zeroes arg1+0x8A, fires func_001FBD50(self,0x7EA,0,300.0f) and
//   (if func_001EFE00(0x8000001E,self)==0) sets self+4 = 3.
// State 2: idle.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 84.6%. Keys: anim_clip_init's real signature is (self, clip, float, float) -- the
// trailing 0.0f emits the mtc1 zero,$f13 the target has; ascending switch cases
// reproduce the target's 2->1->0 descending dispatch with the case-0 branch-likely
// (beql). objdiff 100.0 vs expected.
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001B1190(unsigned char a);
extern int func_001EFE00(int a, char *p);
extern void func_001FBD50(char *self, int idx, int flags, float f);

void func_0014ACC0(char *arg0, char *arg1) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(float *)(arg1 + 0x40) = 1.0f;
        *(char *)(arg1 + 0x80) = 0;
        *(char *)(arg1 + 0x88) = 0;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        func_001B1190(*(unsigned char *)(arg0 + 0x9A));
        func_001FBD50(arg0, 0x7EB, 0, 300.0f);
        if (*(char *)(arg1 + 0x87) != 0) {
            anim_clip_init(arg0, 0x2F, 1.0f, 0.0f);
        } else {
            anim_clip_init(arg0, 0x31, 1.0f, 0.0f);
        }
        break;
    case 1:
        if ((*(short *)(arg0 + 0x2C) & 0xFFFF7FFF) == 0x31 && *(float *)(arg0 + 0x3C) == 54.0f) {
            func_001FBD50(arg0, 0x7E8, 0, 300.0f);
        }
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(char *)(arg1 + 0x8A) = 0;
            func_001FBD50(arg0, 0x7EA, 0, 300.0f);
            if (func_001EFE00(0x8000001E, arg0) == 0) {
                *(char *)(arg0 + 4) = 3;
            }
        }
        break;
    case 2:
        break;
    }
}
