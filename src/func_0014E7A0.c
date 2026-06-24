// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Computes a clamped facing angle for an actor and writes it to a target
// struct. Takes the actor (arg0) and a target (arg1). Forms an angle via
// func_001B1470(func_0011E620(dy, dx)) where dy = D_00810368 - actor[+0xB8]
// and dx = D_00810360 - actor[+0xB0] (atan2-style). Stores the raw angle to
// arg1+0x38, then clamps to [-pi/2, +pi/2]: if angle > +pi/2 store +pi/2
// (0x3FC90FDB), else if angle < -pi/2 store -pi/2 (0xBFC90FDB). The clamp
// constants are written as raw int bit-patterns (sw), matching the original
// codegen. Then builds two transforms (func_00102948 from actor+0xB0 and
// from D_00810360), zeroes scratch fields at 0x700038A4/0x700038B4, and
// writes func_001B15D0(...) result to arg1+0x34.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202.
// Verified objdiff 100% vs build/expected/func_0014E7A0.o (991202: 93.23%,
// clean-store delay-slot family wall #13). The if/else (default +pi/2 in the
// `>pi/2` arm, -pi/2 conditional in the else arm) keeps the two clamp stores
// as distinct sites, matching the target's `b`/`sw` + `bc1f`/`sw` shape.
extern int func_00102948(void *, float *);
extern float func_0011E620(float, float);
extern float func_001B1470(float);
extern float func_001B15D0(void *, void *);
extern float D_00810360;
extern float D_00810368;
extern char D_700038A0;
extern char D_700038B0;

void func_0014E7A0(char *arg0, char *arg1) {
    float temp_f0;
    int var_v0;

    temp_f0 = func_001B1470(func_0011E620(D_00810368 - *(float *)(arg0 + 0xB8), D_00810360 - *(float *)(arg0 + 0xB0)));
    var_v0 = 0x3FC90FDB;
    *(float *)(arg1 + 0x38) = temp_f0;
    if (!(temp_f0 <= 1.5707964f)) {
        *(int *)(arg1 + 0x38) = var_v0;
    } else {
        var_v0 = 0xBFC90FDB;
        if (temp_f0 < -1.5707964f) {
            *(int *)(arg1 + 0x38) = var_v0;
        }
    }
    func_00102948(&D_700038A0, (float *)(arg0 + 0xB0));
    func_00102948(&D_700038B0, &D_00810360);
    *(int *)0x700038B4 = 0;
    *(int *)0x700038A4 = 0;
    *(float *)(arg1 + 0x34) = func_001B15D0(&D_700038A0, &D_700038B0);
}
