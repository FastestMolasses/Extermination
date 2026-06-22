// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Spawns up to 3 child objects via func_001AFA90(8). For each non-NULL child it
// copies three vec3-ish fields from the parent (a0): +0xB0, +0xA0 (both fed by
// parent +0xB0), and +0xC0; subtracts 6.0f from the child's +0xB4 and +0xA4
// floats; sets the child's type byte (+0x3)=0x11, index byte (+0xD)=loop i,
// think-function pointer (+0x10)=func_001BF3C0, and owner (+0x20)=parent.
// Returns the count of children actually spawned.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202. Local
// declaration order is load-bearing for saved-register allocation: `i` must be
// declared before `count` (i->s0, count->s1, parent->s2, child->s3) to match the
// target. The 6.0f constant lands in f1 under 2.3.3. Under 991202 the
// function-pointer store and beql delay-slot handling leave a residual; 2.3.3 is
// byte-identical. Verified objdiff 100.0% vs build/expected/func_001BEB70.o.
extern int func_001AFA90(int);
extern void func_00102948(int, int);
extern void func_001BF3C0(void);

int func_001BEB70(int a0) {
    char *s3;
    int i;
    int count;

    count = 0;
    i = 0;
    do {
        s3 = (char *)func_001AFA90(8);
        if (s3) {
            func_00102948((int)(s3 + 0xB0), a0 + 0xB0);
            func_00102948((int)(s3 + 0xA0), a0 + 0xB0);
            *(float *)(s3 + 0xB4) = *(float *)(s3 + 0xB4) - 6.0f;
            *(float *)(s3 + 0xA4) = *(float *)(s3 + 0xA4) - 6.0f;
            func_00102948((int)(s3 + 0xC0), a0 + 0xC0);
            s3[3] = 0x11;
            s3[0xD] = i;
            *(int *)(s3 + 0x10) = (int)func_001BF3C0;
            count += 1;
            *(int *)(s3 + 0x20) = a0;
        }
        i += 1;
    } while (i < 3);
    return count;
}
