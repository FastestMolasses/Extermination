// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Align the player's position mirrors by a shared translation delta.
// target - actor+A0 is added to actor+A0, actor+B0 and scratch3B40.
// C0 is the Euler vector: copy it to scratch3B50 without translating it.
// The SDK vector helpers operate on four floats; this is not a matrix.
extern void func_001028D0(void *out, void *left, void *right);
extern void func_001028B8(void *out, void *left, void *right);
extern void func_00102948(void *out, void *source);
extern char D_70003B40[];
extern char D_70003B50[];

void func_00182F90(char *actor, void *target) {
    char delta[16];
    func_001028D0(delta, target, actor + 0xA0);
    func_001028B8(actor + 0xA0, actor + 0xA0, delta);
    func_001028B8(actor + 0xB0, actor + 0xB0, delta);
    func_001028B8(D_70003B40, D_70003B40, delta);
    func_00102948(D_70003B50, actor + 0xC0);
}
