// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state animation driver dispatched on the state byte at arg0+5 (3-way
// switch 0/1/2; any other value falls through to the tail). State 0: bumps the
// state, primes the anim block at arg1 (sets the 1.0f field at +0x28, zeroes
// +0x30/+0x34/+0x38/+0x3C), then -- depending on whether the short at arg0+0x34
// is set and the 0x8000 flag at arg0+0x36 -- kicks clip 0x16 or 0x17 via
// anim_clip_init(self, clip, 5.0f, 0.0f) and func_001FBD50(self, 0x893/0x894,
// 0, 500.0f); clears byte +5 of the linked object at arg0+0x20 if present.
// State 1: only while the 0x1000 flag at arg1+0 is set; if the short at
// arg0+0x34 is set it latches arg0+4=1, resets the state/byte+6, and stores a
// timer value (((func_00122BB8()>>13)&0x3F)+0x3C) at arg1+0x50; otherwise it
// advances the state, sets D_0081080E=2, and marks the linked object at
// arg0+0x20 (+4 = 3). State 2: when D_0081080E has reached 3, sets arg0+4 = 3.
// Tail (always): arg1+0 = anim_advance_time(self, arg1+0x28 float), then
// func_00152EB0/func_001C68C0/func_001B17A0(self) and an indirect call through
// the function pointer at arg0+0x4C.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 91.7%. Key: func_00122BB8 takes no arguments (the target sets up no arg
// registers for it) -- declaring it func_00122BB8() removes the spurious
// `paddub a0` setup and resolves the a1/a2 register coloring on the switch
// scrutinee. anim_clip_init's real signature is (self, clip, float, float); the
// trailing 0.0f emits the `mtc1 zero,$f13` pair-clear the target has.
extern void anim_clip_init(int self, int clip, float a, float b);
extern int anim_advance_time(int self, float t);
extern int func_00122BB8();
extern void func_00152EB0(int self);
extern void func_001C68C0(int self);
extern void func_001B17A0(int self);
extern void func_001FBD50(int self, int a, int b, float f);
extern unsigned char D_0081080E;

void func_00152CC0(int arg0, int arg1) {
    unsigned char st;
    int p;

    st = *(unsigned char *)(arg0 + 5);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 5) = st + 1;
        *(int *)(arg1 + 0x28) = 0x3F800000;
        *(int *)(arg1 + 0x34) = 0;
        *(int *)(arg1 + 0x30) = 0;
        *(int *)(arg1 + 0x3C) = 0;
        *(int *)(arg1 + 0x38) = 0;
        if (*(short *)(arg0 + 0x34) != 0 && (*(short *)(arg0 + 0x36) & 0x8000)) {
            anim_clip_init(arg0, 0x16, 5.0f, 0.0f);
            func_001FBD50(arg0, 0x893, 0, 500.0f);
        } else {
            anim_clip_init(arg0, 0x17, 5.0f, 0.0f);
            func_001FBD50(arg0, 0x894, 0, 500.0f);
        }
        p = *(int *)(arg0 + 0x20);
        if (p != 0) {
            *(char *)(p + 5) = 0;
        }
        break;
    case 1:
        if (*(int *)(arg1 + 0) & 0x1000) {
            if (*(short *)(arg0 + 0x34) != 0) {
                *(char *)(arg0 + 4) = 1;
                *(unsigned char *)(arg0 + 5) = 0;
                *(char *)(arg0 + 6) = 0;
                *(char *)(arg1 + 0x50) = ((func_00122BB8() >> 0xD) & 0x3F) + 0x3C;
            } else {
                *(unsigned char *)(arg0 + 5) = st + 1;
                D_0081080E = 2;
                p = *(int *)(arg0 + 0x20);
                if (p != 0) {
                    *(char *)(p + 4) = 3;
                }
            }
        }
        break;
    case 2:
        if (D_0081080E == 3) {
            *(char *)(arg0 + 4) = 3;
        }
        break;
    }
    *(int *)(arg1 + 0) = anim_advance_time(arg0, *(float *)(arg1 + 0x28));
    func_00152EB0(arg0);
    func_001C68C0(arg0);
    func_001B17A0(arg0);
    (*(void (**)(int))(arg0 + 0x4C))(arg0);
}
