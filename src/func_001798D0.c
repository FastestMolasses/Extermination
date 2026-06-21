// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Actor (re)spawn/clear on an actor struct (unsigned char *): zeroes the
// +0x38 and +0x21C word fields and the +0x25C byte, calls func_00174A50 with
// the actor pointer and a float 0.0 (passed in $f12 via mtc1 $zero), then
// clears the +0x5, +0x6 and +0x1F0 flag bytes.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 991202
// reschedules the pre-call stores / delay slot differently (wall #13). The
// 2.3.3 build is byte-identical (.text cmp vs build/expected) → readable plain
// C, no asm void.
extern void func_00174A50(unsigned char *, float);

void func_001798D0(unsigned char *arg0) {
    *(int *)(arg0 + 0x38) = 0;
    *(int *)(arg0 + 0x21C) = 0;
    arg0[0x25C] = 0;
    func_00174A50(arg0, 0.0f);
    arg0[0x5] = 0;
    arg0[0x6] = 0;
    arg0[0x1F0] = 0;
}
