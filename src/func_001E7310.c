// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Entity sub-state machine dispatched on the state byte self[4] (channel 9 effect).
//   case 0: init -- state=1, +0x60 (intensity) = 0.5, +0x64 (amplitude) = 64.0,
//           open the effect channel func_001D2830(9, 1) and bind its parameter
//           block func_001DEE80(9, &D_0026E970); falls through to case 1.
//   case 1: per-frame update -- push the integer amplitude func_001DEEC0(9, (int)+0x64),
//           push the scaled intensity func_001DF5A0(0.2 * +0x60), decay the amplitude
//           by 0.98/frame and ease the intensity toward 0 with a proportional term
//           plus a constant bleed (0.05*-x - 0.015); once it reaches 0 advance to
//           state 3 (teardown).
//   case 2/3: teardown -- close the channel func_001D2830(9, 0) and release the
//           entity func_001AFC10(self).
// The case-0 call reuses the dispatch literal 1 as func_001D2830's second argument.
extern int float_to_int(float);
extern void func_001D2830(int, int);
extern void func_001DEE80(int, void *);
extern void func_001DEEC0(int, int);
extern void func_001DF5A0(float);
extern void func_001AFC10(unsigned char *);
extern unsigned char D_0026E970;

void func_001E7310(unsigned char *self) {
    switch (self[4]) {
    case 0:
        self[4] = 1;
        *(float *)(self + 0x60) = 0.5f;
        *(float *)(self + 0x64) = 64.0f;
        func_001D2830(9, 1);
        func_001DEE80(9, &D_0026E970);
    case 1:
        func_001DEEC0(9, float_to_int(*(float *)(self + 0x64)));
        func_001DF5A0(0.2f * *(float *)(self + 0x60));
        *(float *)(self + 0x64) *= 0.98f;
        *(float *)(self + 0x60) += 0.05f * -*(float *)(self + 0x60) - 0.015f;
        if (*(float *)(self + 0x60) <= 0.0f) { self[4] = 3; }
        break;
    case 2:
    case 3:
        func_001D2830(9, 0);
        func_001AFC10(self);
        break;
    }
}
