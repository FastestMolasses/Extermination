// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Calls func_001EF9D0(arg0, arg1+0x30, 1.0f) (f12=1.0); on a non-null result
// copies a quadword block via copy_qw4(result+0xD0, arg1) and returns the
// result pointer. The float arg is written FIRST in the prototype so mwcc
// materializes mtc1 f12 before the addiu a1 arg-setup (scheduler order match);
// f12 is the first FPU arg slot regardless of textual position.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 is wall #13 (it fills the `beqz s0` clean delay slot;
// CW/2.3.3 leave the nop). 2.3.3 is byte-identical (objdiff 100%).
extern char *func_001EF9D0(float, int, int);
extern void copy_qw4(int, int);

char *func_001EFEB0(int arg0, int arg1) {
    char *v0;

    v0 = func_001EF9D0(1.0f, arg0, arg1 + 0x30);
    if (v0 != 0) {
        copy_qw4((int)(v0 + 0xD0), arg1);
    }
    return v0;
}
