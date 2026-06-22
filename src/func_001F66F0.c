// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Release-handles sweep over a 0x28-stride record array starting at `self`.
// Walks records while the leading signed short (record+0x0) is >= 0 (the
// list terminator/sentinel is a negative short). For each live record whose
// handle field (record+0x24) is not -1, releases it via func_001D80B0(handle)
// and marks the slot free by writing -1 back to record+0x24. A NULL `self`
// is a no-op.
//
// Matched with mwcc 2.3.3 (mwccps2-2.3.3-000906). The early-return guard
// reproduces CW's loop rotation (entry jumps straight to the bottom
// condition test). 991202 leaves a residual delay-slot wall; 2.3.3 is
// byte-identical (objdiff 100%).
extern void func_001D80B0(int handle);

void func_001F66F0(unsigned char *self) {
    if (self == 0) {
        return;
    }
    while (*(short *)self >= 0) {
        if (*(int *)(self + 0x24) != -1) {
            func_001D80B0(*(int *)(self + 0x24));
            *(int *)(self + 0x24) = -1;
        }
        self += 0x28;
    }
}
