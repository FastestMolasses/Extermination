// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// HUD/menu row-drawing dispatcher (sibling of func_00201290): builds the D_00282240 GS
// header via func_00207D00(1,3)/func_001FCE30, then a 9-row menu (rows keyed 0x47..0xA7)
// using arg1 as the "which row is highlighted" state and arg2 as an extra flag that gates
// each row's 0x155 detail sub-row. Row 0x47 (state 7) and rows 0x6B/0x77/0x8F/0x9B(state
// header)/0xA7(header) are single func_001FCBD0 calls; rows 0x53/0x5F additionally carry a
// color (0x80808080 highlighted / 0x40404040 dim) into a 0x155 detail sub-row keyed off
// arg0[1]/arg0[4]; row 0x83 always draws its header then a 3-way arg0[0] (0/1/else) detail
// sub-row; rows 0x9B/0xA7 draw a detail sub-row only when arg2!=0, keyed off arg0[3] /
// *(*(unsigned char**)0x70003B6C)[0x13]. Ends with one packed func_00207E40 GIF register
// write carrying the 64-bit immediate 0x20045505DD421D40.
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches 75.2%.
extern int func_001FCE30(int, int, int, int, unsigned int);
extern void func_001FCBD0(int, int, int, int, unsigned int);
extern void func_00207D00(int, int);
extern void func_00207E40(int, int, int, int, int, int, unsigned long long);
extern int D_00282240;
extern unsigned short D_00810E6A;

void func_0022AEA0(unsigned char *arg0, int arg1, int arg2) {
    int s0;
    unsigned int c;
    int v1;

    s0 = D_00282240;
    func_00207D00(1, 3);
    func_001FCE30(0xD0, 0x25, s0, 0, 0x80808080);
    if (arg1 == 7) {
        func_001FCBD0(0x6F, 0x47, s0, 8, 0x80808080);
    } else {
        func_001FCBD0(0x6F, 0x47, s0, 8, 0x40404040);
    }
    if (arg1 == 0) {
        if (D_00810E6A != 7) {
            func_001FCBD0(0x6F, 0x53, s0, 1, 0x40404040);
            c = 0x40404040;
        } else {
            func_001FCBD0(0x6F, 0x53, s0, 1, 0x80808080);
            if (arg2 != 0) {
                c = 0x80808080;
            } else {
                c = 0x40404040;
            }
        }
    } else {
        func_001FCBD0(0x6F, 0x53, s0, 1, 0x40404040);
        c = 0x40404040;
    }
    if (arg0[1] == 1) {
        func_001FCBD0(0x155, 0x53, s0, 0x28, c);
    } else {
        func_001FCBD0(0x155, 0x53, s0, 0x29, c);
    }
    if (arg1 == 1) {
        func_001FCBD0(0x6F, 0x5F, s0, 0xC, 0x80808080);
        if (arg2 != 0) {
            c = 0x80808080;
        } else {
            c = 0x40404040;
        }
    } else {
        func_001FCBD0(0x6F, 0x5F, s0, 0xC, 0x40404040);
        c = 0x40404040;
    }
    if (arg0[4] == 0) {
        func_001FCBD0(0x155, 0x5F, s0, 2, c);
    } else {
        func_001FCBD0(0x155, 0x5F, s0, 3, c);
    }
    if (arg1 == 2) {
        func_001FCBD0(0x6F, 0x6B, s0, 0x17, 0x80808080);
    } else {
        func_001FCBD0(0x6F, 0x6B, s0, 0x17, 0x40404040);
    }
    if (arg1 == 3) {
        func_001FCBD0(0x6F, 0x77, s0, 0x22, 0x80808080);
    } else {
        func_001FCBD0(0x6F, 0x77, s0, 0x22, 0x40404040);
    }
    if (arg1 == 4) {
        func_001FCBD0(0x6F, 0x83, s0, 0x25, 0x80808080);
    } else {
        func_001FCBD0(0x6F, 0x83, s0, 0x25, 0x40404040);
    }
    v1 = arg0[0];
    if (v1 == 0) {
        func_001FCBD0(0x155, 0x83, s0, 4, 0x40404040);
    } else if (v1 == 1) {
        func_001FCBD0(0x155, 0x83, s0, 5, 0x40404040);
    } else {
        func_001FCBD0(0x155, 0x83, s0, 6, 0x40404040);
    }
    if (arg1 == 9) {
        func_001FCBD0(0x6F, 0x8F, s0, 0xB, 0x80808080);
    } else {
        func_001FCBD0(0x6F, 0x8F, s0, 0xB, 0x40404040);
    }
    if (arg1 == 5) {
        func_001FCBD0(0x6F, 0x9B, s0, 0x26, 0x80808080);
        if (arg2 != 0) {
            if (arg0[3] == 0) {
                func_001FCBD0(0x155, 0x9B, s0, 0x21, 0x80808080);
            } else {
                func_001FCBD0(0x155, 0x9B, s0, 0x23, 0x80808080);
            }
        }
    } else {
        func_001FCBD0(0x6F, 0x9B, s0, 0x26, 0x40404040);
    }
    if (arg1 == 8) {
        func_001FCBD0(0x6F, 0xA7, s0, 0xE, 0x80808080);
        if (arg2 != 0) {
            if ((*(unsigned char **)0x70003B6C)[0x13] == 0) {
                func_001FCBD0(0x155, 0xA7, s0, 0x21, 0x80808080);
            } else {
                func_001FCBD0(0x155, 0xA7, s0, 0x23, 0x80808080);
            }
        }
    } else {
        func_001FCBD0(0x6F, 0xA7, s0, 0xE, 0x40404040);
    }
    func_00207E40(1, 0x7000, 0x8300, 0x80, 0x80, 0x80808080, 0x20045505DD421D40ULL);
}
