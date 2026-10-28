// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Steering decision for an actor turning toward a target direction. The actor's
// forward axis (0, 0, 1, 1) is rotated by its orientation (func_001B2B10); the
// target vector is copied, flattened (y = 0) and normalized. If the angle between
// them (acos of the dot product, func_0011E420) is at most 3*pi/4 (2.3561945), the
// direction is offset by the actor position (+0xB0, func_001028B8) and the turn
// is decided by func_001B1380(dir, position, heading +0xC4). Otherwise a probe
// point straight ahead at half the distance func_001B15D0(position, arg2)
// returns is rotated and offset the same way and func_001B39F0(actor, probe,
// dir) decides. Returns the callee's result as a signed char.
//
// Note: the original returns func_001B1380's value without narrowing it, so
// that callee really returns char (its src/ definition says int); declared
// here as char so the return path matches.
extern void func_001B2B10(unsigned char *a, float *b, float *c);
extern void func_00102948(float *dst, float *src);
extern void func_00102760(float *dst, float *v);
extern float func_00102738(float *a, float *b);
extern float func_0011E420(float x);
extern void func_001028B8(float *dst, float *a, unsigned char *b);
extern char func_001B1380(float *a, unsigned char *b, float heading);
extern float func_001B15D0(unsigned char *a, float *b);
extern int func_001B39F0(unsigned char *a, float *b, float *c);

char func_001B3670(unsigned char *self, float *target, float *arg2) {
    float fwd[4];
    float dir[4];

    fwd[0] = 0.0f;
    fwd[1] = 0.0f;
    fwd[2] = 1.0f;
    fwd[3] = 1.0f;
    func_001B2B10(self, fwd, fwd);
    func_00102948(dir, target);
    dir[1] = 0.0f;
    func_00102760(dir, dir);
    if (func_0011E420(func_00102738(fwd, dir)) <= 2.3561945f) {
        func_001028B8(dir, dir, self + 0xB0);
        return func_001B1380(dir, self + 0xB0, *(float *)(self + 0xC4));
    }
    fwd[2] = 0.0f;
    fwd[1] = 0.0f;
    fwd[0] = 0.0f;
    fwd[3] = 1.0f;
    fwd[2] = func_001B15D0(self + 0xB0, arg2) / 2.0f;
    func_001B2B10(self, fwd, fwd);
    func_001028B8(fwd, fwd, self + 0xB0);
    return func_001B39F0(self, fwd, dir);
}
