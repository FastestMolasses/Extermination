// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Per-frame tick on the global object at D_008102B0 (gated by D_008102B1).
// Calls func_001CB590(self, 0x320, self[9]); then, unless D_00810771 == 1,
// either re-arms via func_001DA6A0(D_00275B44) (when self->w[0x214]==0) or
// runs func_0015BF90(self). Finally dispatches the hook function pointer at
// self+0x4C with arg D_00275B44. D_00275B44 is gp-relative; D_008102B0/B1 and
// D_00810771 are far (absolute), over-declared as arrays to force the
// absolute lui/lo addressing under -sdatathreshold 8.
extern void func_001CB590(unsigned char *a0, int a1, int a2);
extern void func_001DA6A0(int a0);
extern void func_0015BF90(unsigned char *a0);
extern int D_00275B44;
extern unsigned char D_008102B0[64];
extern unsigned char D_008102B1[64];
extern unsigned char D_00810771[64];

void func_0015C160(void) {
    unsigned char *s = D_008102B0;
    void (*hook)(int);

    if (D_008102B1[0] == 0) {
        return;
    }
    func_001CB590(s, 0x320, s[9]);
    if (D_00810771[0] != 1) {
        if (*(int *)(s + 0x214) == 0) {
            func_001DA6A0(D_00275B44);
        } else {
            func_0015BF90(s);
        }
    }
    hook = *(void (**)(int))(s + 0x4C);
    hook(D_00275B44);
}
