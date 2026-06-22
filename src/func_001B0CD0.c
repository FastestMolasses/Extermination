// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Spawns/acquires an actor via func_001F4F40(0); if non-null, copies the
// source actor's launch quad (+0xB0, copy_qw via func_00102948) into the new
// actor, copies a second quad (+0xD0) from the source's +0x110 pointer + 0x90
// (copy_qw4), then runs func_001026A0 to transform that +0xD0 quad by one of
// four 0x10-byte matrices/vectors from table D_0024D6B0 (selected by arg1 & 3)
// into a scratch quad, which is stored to the new actor at +0x100..+0x10C.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the lone residual is wall #13 (it fills the `beqz s0` delay slot
// where CW leaves a nop). 2.3.3 leaves the nop, so this readable C is
// byte-identical. objdiff 100.0% vs expected.
extern char *func_001F4F40(int);
extern void func_00102948(char *, char *);
extern void copy_qw4(char *, char *);
extern void func_001026A0(float *, char *, char *);
extern char D_0024D6B0[];

void func_001B0CD0(char *arg0, int arg1) {
    float sp40[4];
    char *v0;

    v0 = func_001F4F40(0);
    if (v0 != 0) {
        func_00102948(v0 + 0xB0, arg0 + 0xB0);
        copy_qw4(v0 + 0xD0, *(char **)(arg0 + 0x110) + 0x90);
        func_001026A0(sp40, v0 + 0xD0, &D_0024D6B0[(arg1 & 3) * 0x10]);
        *(float *)(v0 + 0x100) = sp40[0];
        *(float *)(v0 + 0x104) = sp40[1];
        *(float *)(v0 + 0x108) = sp40[2];
        *(float *)(v0 + 0x10C) = sp40[3];
    }
}
