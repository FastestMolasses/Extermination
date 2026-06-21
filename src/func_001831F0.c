// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// 3-way state setter on the global struct D_008102B0: sets a byte flag at
// +0x23F and an int at +0x24C according to the mode arg.
//   mode 0  -> flag=0, value=-1
//   mode 1  -> flag=2, value=0
//   default -> flag=2, value=1
// The original holds the struct base in one register (a2 = D_008102B0) and
// addresses both fields via constant load offsets — hence the `char *p` base
// pointer rather than D_008102B0 + off (which would fold %hi(base+off)).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 991202's
// branch/delay-slot lowering of the bnez/bne chain diverges (wall #13 family),
// capping at 84.21%. 2.3.3 reproduces the CW delay-slot fill → objdiff 100.0.
extern char D_008102B0[];

void func_001831F0(int a0) {
    char *p = D_008102B0;
    if (a0 == 0) {
        *(char *)(p + 0x23F) = 0;
        *(int *)(p + 0x24C) = -1;
    } else if (a0 == 1) {
        *(char *)(p + 0x23F) = 2;
        *(int *)(p + 0x24C) = 0;
    } else {
        *(char *)(p + 0x23F) = 2;
        *(int *)(p + 0x24C) = 1;
    }
}
