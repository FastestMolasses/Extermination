// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Per-frame state machine for an actor (a0) driven by an input/spec struct (a1).
// If a1[4] != 1, reset state byte a0[6]=0 and zero the global timeline accumulator
// (*(int*)(*(int*)((char*)D_00275B40+8) + 0x70)). Then dispatch on the state byte
// a0[6]: state 0 advances to 1 when a1[0x275]==2 && a1[0x274]!=0; state 1 advances
// to 2 (and seeds the 0x14 short counter at a0+0x28 + the 0.05236f bits at a0+0x38)
// when the float at a1+0x3C <= 20.0f; state 2 decrements the a0+0x28 counter and,
// when it underflows from 0, resets state and zeroes the accumulator, otherwise
// adds the per-frame delta a0[0x38] into the accumulator.
// Matched with mwcc 2.3.3; the pinned 991202 build only reaches 95.08% (FP add
// operand/register pairing in the state-2 else-branch). objdiff 100.0 vs expected.
extern int *D_00275B40;

void func_00189630(unsigned char *a0, unsigned char *a1)
{
    int state;
    int *p;
    short t;

    if (a1[4] != 1) {
        a0[6] = 0;
        p = (int *)(*(int *)((char *)D_00275B40 + 8));
        *(int *)((char *)p + 0x70) = 0;
    }
    state = a0[6];
    switch (state) {
    case 0:
        if (a1[0x275] == 2 && a1[0x274] != 0) {
            a0[6] = state + 1;
        }
        break;
    case 1:
        if (*(float *)(a1 + 0x3C) <= 20.0f) {
            a0[6] = state + 1;
            *(short *)(a0 + 0x28) = 0x14;
            *(int *)(a0 + 0x38) = 0x3D567750;
        }
        break;
    case 2:
        t = *(short *)(a0 + 0x28);
        *(short *)(a0 + 0x28) = t - 1;
        if (t == 0) {
            a0[6] = 0;
            p = (int *)(*(int *)((char *)D_00275B40 + 8));
            *(int *)((char *)p + 0x70) = 0;
        } else {
            float d;
            d = *(float *)(a0 + 0x38);
            p = (int *)(*(int *)((char *)D_00275B40 + 8));
            *(float *)((char *)p + 0x70) =
                *(float *)((char *)p + 0x70) + d;
        }
        break;
    }
}
