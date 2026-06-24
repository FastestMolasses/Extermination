// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// HUD/menu register-setup dispatcher. Issues func_00207D00(1,3), a func_001FCE30
// header upload, then a fixed run of func_001FCBD0 GS/GIF register writes for a
// 7-row menu (rows keyed 0x47..0x8F). For each row the 5th arg (t0) is a packed
// color constant that is 0x80808080 (highlighted) when arg1 selects that row,
// else 0x40404040 (dim); the two 0x155 sub-rows inherit the color carried out of
// the preceding row's block (variable c). arg0[0]/[1]/[3]/[4] select per-row
// glyph variants. Closes with one func_00207E40 packed register write.
// Matched with mwcc 2.3.3: the carried-color if/else shape (NOT a ternary, which
// mwcc lowers to movz) reproduces CW's branch-based color selection; 991202
// reaches 73.9% (delay-slot/branch lowering wall). Verified objdiff 100.0.
extern int func_001FCE30(int, int, int, int, unsigned int);
extern void func_001FCBD0(int, int, int, int, unsigned int);
extern void func_00207D00(int, int);
extern void func_00207E40(int, int, int, int, int, int, unsigned long long);
extern int D_00282240;
extern unsigned short D_00810E6A;

void func_00201290(unsigned char *arg0, int arg1, int arg2) {
    int s0;
    unsigned int c;

    s0 = D_00282240;
    func_00207D00(1, 3);
    func_001FCE30(0xD0, 0x25, s0, 0, 0x80808080);
    if (arg1 == 0) {
        if (D_00810E6A != 7) {
            func_001FCBD0(0x6F, 0x47, s0, 1, 0x40404040);
            c = 0x40404040;
        } else {
            func_001FCBD0(0x6F, 0x47, s0, 1, 0x80808080);
            if (arg2 != 0) {
                c = 0x80808080;
            } else {
                c = 0x40404040;
            }
        }
    } else {
        func_001FCBD0(0x6F, 0x47, s0, 1, 0x40404040);
        c = 0x40404040;
    }
    if (arg0[1] == 1) {
        func_001FCBD0(0x155, 0x47, s0, 0x28, c);
    } else {
        func_001FCBD0(0x155, 0x47, s0, 0x29, c);
    }
    if (arg1 == 1) {
        func_001FCBD0(0x6F, 0x53, s0, 0xC, 0x80808080);
        if (arg2 != 0) {
            c = 0x80808080;
        } else {
            c = 0x40404040;
        }
    } else {
        func_001FCBD0(0x6F, 0x53, s0, 0xC, 0x40404040);
        c = 0x40404040;
    }
    if (arg0[4] == 0) {
        func_001FCBD0(0x155, 0x53, s0, 2, c);
    } else {
        func_001FCBD0(0x155, 0x53, s0, 3, c);
    }
    if (arg1 == 2) {
        func_001FCBD0(0x6F, 0x5F, s0, 0x17, 0x80808080);
    } else {
        func_001FCBD0(0x6F, 0x5F, s0, 0x17, 0x40404040);
    }
    if (arg1 == 3) {
        func_001FCBD0(0x6F, 0x6B, s0, 0x22, 0x80808080);
    } else {
        func_001FCBD0(0x6F, 0x6B, s0, 0x22, 0x40404040);
    }
    if (arg1 == 4) {
        func_001FCBD0(0x6F, 0x77, s0, 0x25, 0x80808080);
    } else {
        func_001FCBD0(0x6F, 0x77, s0, 0x25, 0x40404040);
    }
    if (arg0[0] == 0) {
        func_001FCBD0(0x155, 0x77, s0, 4, 0x40404040);
    } else if (arg0[0] == 1) {
        func_001FCBD0(0x155, 0x77, s0, 5, 0x40404040);
    } else {
        func_001FCBD0(0x155, 0x77, s0, 6, 0x40404040);
    }
    if (arg1 == 5) {
        func_001FCBD0(0x6F, 0x83, s0, 0x26, 0x80808080);
        if (arg2 != 0) {
            if (arg0[3] == 0) {
                func_001FCBD0(0x155, 0x83, s0, 0x21, 0x80808080);
            } else {
                func_001FCBD0(0x155, 0x83, s0, 0x23, 0x80808080);
            }
        }
    } else {
        func_001FCBD0(0x6F, 0x83, s0, 0x26, 0x40404040);
    }
    if (arg1 == 6) {
        func_001FCBD0(0x6F, 0x8F, s0, 0x27, 0x80808080);
    } else {
        func_001FCBD0(0x6F, 0x8F, s0, 0x27, 0x40404040);
    }
    func_00207E40(1, 0x7000, 0x8300, 0x80, 0x80, 0x80808080, 0x20043845DD422100ULL);
}
