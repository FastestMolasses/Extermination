// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Actor init helper: calls func_001B0F60(actor, 0); on success (==0) wires up
// the actor's radius/desc table pointer (D_002755F0, gp-relative), captures the
// prior 0x2E byte into the 0x34 short and clears 0x2E, then selects a uniform
// scale triple at +0x80/+0x84/+0x88 from the flag word at +0x56:
//   bit 0x40 -> 1.5, else bit 0x80 -> 2.0, else 1.0.
// The float stores are raw lui/sw of the bit patterns (0x3FC00000/0x40000000/
// 0x3F800000) -- writing float literals to a float* reproduces them exactly.
// 991202 cannot match this: it fills the beqz delay slot / lowers the float
// branch block differently; mwcc 2.3.3 is byte-identical.
extern int func_001B0F60(void *p, int a);
extern int D_002755F0;

void func_001BBDA0(unsigned char *p) {
    short f;

    if (func_001B0F60(p, 0) != 0) {
        return;
    }
    *(int *)(p + 0x30) = (int)&D_002755F0;
    *(short *)(p + 0x34) = *(unsigned char *)(p + 0x2E);
    *(short *)(p + 0x2E) = 0;
    f = *(short *)(p + 0x56);
    if (f & 0x40) {
        *(float *)(p + 0x80) = 1.5f;
        *(float *)(p + 0x84) = 1.5f;
        *(float *)(p + 0x88) = 1.5f;
    } else if (f & 0x80) {
        *(float *)(p + 0x80) = 2.0f;
        *(float *)(p + 0x84) = 2.0f;
        *(float *)(p + 0x88) = 2.0f;
    } else {
        *(float *)(p + 0x80) = 1.0f;
        *(float *)(p + 0x84) = 1.0f;
        *(float *)(p + 0x88) = 1.0f;
    }
}
