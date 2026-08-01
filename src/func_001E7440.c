// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Entity sub-state machine on the state byte self[4] (structural twin of
// func_001E7310 / func_001E7570, different tuning constants).
//   case 0: init — state=1, +0x60=1.5, +0x64=128.0, func_001D2830(9,1),
//           func_001DEE80(9,&D_0026E980); falls through to case 1.
//   case 1: func_001DEEC0(9, (int)+0x64); func_001DF5A0(0.2 * +0x60);
//           +0x64 *= 0.98; ease +0x60 by (0.05*-x - 0.145); when the eased
//           value drops to <= 0 the state advances to 3.
//   case 2/3: func_001D2830(9,0) + func_001AFC10(self) (teardown).
extern int float_to_int(float);
extern void func_001D2830(int, int);
extern void func_001DEE80(int, void *);
extern void func_001DEEC0(int, int);
extern void func_001DF5A0(float);
extern void func_001AFC10(unsigned char *);
extern unsigned char D_0026E980;

void func_001E7440(unsigned char *self) {
    switch (self[4]) {
    case 0:
        self[4] = 1;
        *(float *)(self + 0x60) = 1.5f;
        *(float *)(self + 0x64) = 128.0f;
        func_001D2830(9, 1);
        func_001DEE80(9, &D_0026E980);
        /* fallthrough */
    case 1:
        func_001DEEC0(9, float_to_int(*(float *)(self + 0x64)));
        func_001DF5A0(0.2f * *(float *)(self + 0x60));
        *(float *)(self + 0x64) *= 0.98f;
        {
            float x = *(float *)(self + 0x60);
            float nx = -x;
            *(float *)(self + 0x60) = x + (0.05f * nx - 0.145f);
            if (*(float *)(self + 0x60) <= 0.0f) {
                self[4] = 3;
            }
        }
        break;
    case 2:
    case 3:
        func_001D2830(9, 0);
        func_001AFC10(self);
        break;
    }
}
