// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001AF890(void *);

// Releases a handle stored at +0x90: if the handle is non-null, hand it to
// func_001AF890, then clear the slot (+0x90 = 0) and reset the short at +0x94 to -1.
// mwcc 991202 mis-schedules this case (delays addiu v1,zero,-1 until after the
// sw zero,0x90(s0) store -> 87.5%); mwcc 2.3.3 emits the target ordering -> 100.0.
void func_001CA770(char *a0) {
    void *h = *(void **)(a0 + 0x90);
    if (h) {
        func_001AF890(h);
        *(int *)(a0 + 0x90) = 0;
        *(short *)(a0 + 0x94) = -1;
    }
}
