// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA16 overlay, runtime 0x00827030 (splat/link name 00826FF0; overlay code is
// linked 0x40 below where it runs), 0x3C0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0 (after func_001B0FD0) state 1, +0 = 1, +0x28 = +0x2A = 0,
//  +0x1F0 = 1, +0x1F4 = 1/6. State 1: +0x2A reverses the direction (+0x1F0 =
//  -+0x1F0 sign, +0x1F4 = +-1/6); +0x28 steps by the direction through 0..30;
//  sets +0x84 = 5 on D_00275B40[1..26] and +0x80 = -2.5 on [4..22], then [1]
//  +0x84 = 5 + 5t, [3] +0x80 = -2.5t, [0x17] +0x80 = -2.5 + 2.5t (t = +0x28 /
//  30); animate; when the player (D_00810350) is inside the box for +3
//  (nonzero: x 180..201, y 149.9..200.4, z 124..239.7; zero: x 205..226, y
//  99.9..152, z 122..236) sets D_700031F0 = 1 and moves z by +-+0x1F4. State
//  3/other func_001AFC10.
extern unsigned char **D_00275B40;
extern float D_00810350[4];
extern int D_700031F0[4];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00826FF0(unsigned char *self) {
    int i;
    float t;
    float b;
    float a;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            self[4] = 1;
            self[0] = 1;
            *(short *)(self + 0x28) = 0;
            *(short *)(self + 0x2A) = 0;
            *(int *)(self + 0x1F0) = 1;
            *(float *)(self + 0x1F4) = 0.16666667f;
        }
        break;
    case 1:
        if (*(short *)(self + 0x2A) != 0) {
            if (*(int *)(self + 0x1F0) > 0) {
                *(int *)(self + 0x1F0) = -1;
                *(float *)(self + 0x1F4) = -0.16666667f;
            } else {
                *(int *)(self + 0x1F0) = 1;
                *(float *)(self + 0x1F4) = 0.16666667f;
            }
            *(short *)(self + 0x2A) = 0;
        }
        /* the int direction at +0x1F0 is read as its low halfword */
        *(short *)(self + 0x28) += (short)*(int *)(self + 0x1F0);
        if (*(short *)(self + 0x28) > 30) {
            *(short *)(self + 0x28) = 0;
        }
        if (*(short *)(self + 0x28) < 0) {
            *(short *)(self + 0x28) = 30;
        }
        t = (float)*(short *)(self + 0x28) / 30.0f;
        a = -(2.5f * t);
        b = 5.0f * t;
        for (i = 1; i < 27; i++) {
            *(float *)(D_00275B40[i] + 0x84) = 5.0f;
        }
        for (i = 4; i < 23; i++) {
            *(float *)(D_00275B40[i] + 0x80) = -2.5f;
        }
        *(float *)(D_00275B40[1] + 0x84) = 5.0f + b;
        *(float *)(D_00275B40[3] + 0x80) = a;
        *(float *)(D_00275B40[0x17] + 0x80) = -2.5f - a;
        func_001C6380(self);
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        if (self[3] != 0) {
            if (D_00810350[1] > 149.9f && D_00810350[1] < 200.4f && D_00810350[0] > 180.0f &&
                D_00810350[0] < 201.0f && D_00810350[2] > 124.0f && D_00810350[2] < 239.7f) {
                t = *(float *)(self + 0x1F4);
                D_700031F0[0] = 1;
                D_00810350[2] = D_00810350[2] + t;
            }
        } else {
            if (D_00810350[1] > 99.9f && D_00810350[1] < 152.0f && D_00810350[0] > 205.0f &&
                D_00810350[0] < 226.0f && D_00810350[2] > 122.0f && D_00810350[2] < 236.0f) {
                t = *(float *)(self + 0x1F4);
                D_700031F0[0] = 1;
                D_00810350[2] = D_00810350[2] - t;
            }
        }
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
