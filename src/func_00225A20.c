// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// State reset for an absolute-addressed global block at 0x008100xx: clears a
// status byte (+0x4A) and eleven 32-bit fields (+0x60..+0x84, +0xA8), then
// memset-zeroes two regions via func_00121A28(ptr, 0, len): D_008100AC (8
// bytes) and D_008100B4 (0x60 bytes). -sdatathreshold 0 forces every global
// to absolute %hi/%lo addressing (the block is far out of gp range).
//
// Built with mwcc 2.3.3: the 991202 residual is instruction scheduling /
// delay-slot fill around the two func_00121A28 calls that CW + 2.3.3 share
// but 991202 reorders. Verified objdiff 100% vs build/expected/func_00225A20.o.
extern void func_00121A28(void *, int, int);
extern unsigned char D_0081004A;
extern int D_00810060;
extern int D_00810064;
extern int D_00810068;
extern int D_0081006C;
extern int D_00810070;
extern int D_00810074;
extern int D_00810078;
extern int D_0081007C;
extern int D_00810080;
extern int D_00810084;
extern int D_008100A8;
extern char D_008100AC;
extern char D_008100B4;

void func_00225A20(void) {
    D_0081004A = 0;
    D_00810060 = 0;
    D_00810064 = 0;
    D_00810068 = 0;
    D_0081006C = 0;
    D_00810070 = 0;
    D_00810074 = 0;
    D_00810078 = 0;
    D_0081007C = 0;
    D_00810080 = 0;
    D_00810084 = 0;
    D_008100A8 = 0;
    func_00121A28(&D_008100AC, 0, 8);
    func_00121A28(&D_008100B4, 0, 0x60);
}
