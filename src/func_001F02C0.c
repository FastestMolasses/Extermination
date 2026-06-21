// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Multi-call non-leaf: saves float callee-save fs0 ($f20), stashes the
// float arg, fills two stack buffers via func_00102948 and func_001FBD50.
// Local declaration order (buf30 before bufE0) reproduces CW's exact
// sp+0x30 / sp+0xE0 frame layout. paddub-zero = register move idiom.
struct S30 { char pad[0xB0]; };
struct SE0 { char pad[0x240]; };

extern void func_00102948(struct SE0 *, int);
extern void func_001FBD50(struct S30 *, int, int, float);

void func_001F02C0(int arg0, int arg1, float f) {
    struct S30 buf30;
    struct SE0 bufE0;
    func_00102948(&bufE0, arg0);
    func_001FBD50(&buf30, arg1, 0, f);
}
