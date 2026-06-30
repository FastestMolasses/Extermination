// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-object update tick gated on the global mode byte D_00810CA5 == 6.
// Computes flag = byte[self+2] with the 0xE0 bits cleared, then dispatches on
// flag (values 2/7/8/10). Each arm conditionally calls func_001B1CE0(self)
// based on the sub-state byte[self+3] (and, for the 0x2D-threshold case, the
// short at self+0x2E). For flag==2 the call fires when byte3==1 with the low
// bit of byte[self+0xD] set, OR when byte3==7. Afterwards it always runs
// func_001B1630 over the three floats at self+0xB0/0xB4/0xB8, stores the u8
// result into byte[self+1], and if nonzero calls func_001B1B70(self), then
// returns byte[self+1].
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// reaches 85.8%. Key: the flag==2 arm must keep the target's fall-through
// branch sense `bnez (byte0xD&1) -> call; else if byte3!=7 break` -- writing
// it as a `goto call` so mwcc emits the forward `bnez ... call` instead of
// inverting to `beqz`.
extern unsigned char func_001B1630(float a, float b, float c);
extern void func_001B1B70(char *self);
extern void func_001B1CE0(char *self);
extern unsigned char D_00810CA5;

unsigned char func_001B17A0(char *self) {
    unsigned char flag;

    flag = (unsigned char)(*(unsigned char *)(self + 2) & ~0xE0);
    if (D_00810CA5 == 6) {
        switch (flag) {
        case 2:
            if (*(unsigned char *)(self + 3) == 1) {
                if (*(unsigned char *)(self + 0xD) & 1) {
                    goto call;
                }
            }
            if (*(unsigned char *)(self + 3) != 7) {
                break;
            }
        call:
            func_001B1CE0(self);
            break;
        case 8:
            if (*(unsigned char *)(self + 3) == 7) {
                func_001B1CE0(self);
            }
            break;
        case 10:
            if (*(unsigned char *)(self + 3) == 1) {
                func_001B1CE0(self);
            }
            break;
        case 7:
            if (*(unsigned char *)(self + 3) == 0) {
                if (*(unsigned short *)(self + 0x2E) >= 0x2D) {
                    func_001B1CE0(self);
                }
            }
            break;
        }
    }
    *(unsigned char *)(self + 1) = func_001B1630(*(float *)(self + 0xB0), *(float *)(self + 0xB4), *(float *)(self + 0xB8));
    if (*(unsigned char *)(self + 1) != 0) {
        func_001B1B70(self);
    }
    return *(unsigned char *)(self + 1);
}
