// COMPILER: eegcc
// CFLAGS: -O2 -fno-schedule-insns
// SDK/eegcc-convention func (sd ra, 16-byte stride, struct@sp+0x0 / float@sp+0x10).
// Spills the float arg, calls func_001278C0(&f, &s) to fill a 2-word out-struct, then
// sets s.b = (s.b < 1)  (unsigned compare -> sltiu v0,v0,1, i.e. s.b becomes 1 iff it was 0),
// and calls func_001277B0(&s). -fno-schedule-insns keeps the ra-save in prologue order.
// (Pure mwcc maxes at ~60% -- emits sq ra + reordered prologue; ee-gcc 2.9 is byte-exact.)
typedef struct { unsigned int a; unsigned int b; } S;
extern void func_001278C0(float *a0, S *a1);
extern void func_001277B0(S *a0);

void func_001282E8(float f) {
    S s;
    float lf;
    lf = f;
    func_001278C0(&lf, &s);
    s.b = (s.b < 1);
    func_001277B0(&s);
}
