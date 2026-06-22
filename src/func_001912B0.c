// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Clamp helper: if a global gate state (D_00810700==0x10, D_00810702==0,
// D_0081078B!=0xFF) holds and the float field at +0xA8 is below 473.0,
// reset that field to 473.0f and run two follow-up handlers.
extern void func_00102948(int, int);
extern void func_00182F90(int, int);
extern unsigned char D_00810700;
extern unsigned char D_00810702;
extern unsigned char D_0081078B;
extern int D_70003600;

void func_001912B0(int a0)
{
    if (D_00810700 == 0x10 && D_00810702 == 0 && D_0081078B != 0xFF &&
        *(float *)(a0 + 0xA8) < 473.0f) {
        *(float *)(a0 + 0xA8) = 473.0f;
        func_00102948((int)&D_70003600, a0 + 0xA0);
        func_00182F90(a0, (int)&D_70003600);
    }
}
