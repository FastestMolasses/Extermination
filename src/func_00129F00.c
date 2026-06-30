// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame charge/recoil update. Adds 0.06 to the actor's charge timer at
// arg1+0xF0, clamping it at 4.0. Subtracts the (clamped) timer from the height
// field at arg0+0xB4. Stages a downward probe vector at the scratch block
// D_700038A0 = {0.0, -5.0, 0.0, 1.0f-as-int} and casts it via func_0019AB20
// (self=arg0, dir=arg0+0xB0, &probe, flags 0x80000007); on a nonzero (hit)
// return the charge timer is reset to 0.06.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); pinned 991202 only reaches
// 87.46% (FP-register-half coloring of the running float). Idiom: writing the
// timer add and the height subtract as `+=` / `-=` read-modify-write makes mwcc
// reuse the same FP register half as CW, resolving the fv0f-vs-fv1 coloring.
extern int func_0019AB20(void *self, void *b, void *c, int id);
extern char D_700038A0[];

void func_00129F00(char *arg0, char *arg1) {
    *(float *)(arg1 + 0xF0) += 0.06f;
    if (!(*(float *)(arg1 + 0xF0) <= 4.0f)) {
        *(float *)(arg1 + 0xF0) = 4.0f;
    }
    *(float *)(arg0 + 0xB4) -= *(float *)(arg1 + 0xF0);
    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A4 = -5.0f;
    *(float *)0x700038A8 = 0.0f;
    *(int *)0x700038AC = 0x3F800000;
    if (func_0019AB20(arg0, arg0 + 0xB0, &D_700038A0, 0x80000007) != 0) {
        *(float *)(arg1 + 0xF0) = 0.06f;
    }
}
