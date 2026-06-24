// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Emits GS/GIF register writes for a HUD element. The byte field p[0x11]
// selects two packed-color constants c0/c1 (0x40808080 / 0x80808080 in various
// pairings). Then, based on the global mode D_00810CA4 (0/1/2/other), issues a
// func_00207E40 GS register write plus, for the non-trivial modes, one or more
// func_00212F30 sprite calls keyed off the per-state palette bytes
// D_00810CA5/CA6/CA7, choosing c0 vs c1 by p[0x11] (==2/==5/==6).
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);
extern void func_00212F30(int a, int color);
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA5;
extern unsigned char D_00810CA6;
extern unsigned char D_00810CA7;

void func_00212B60(unsigned char *p) {
    int c0;
    int c1;

    switch (p[0x11]) {
    case 2:
    case 5:
    case 6:
        c1 = 0x80808080;
        c0 = 0x40808080;
        break;
    case 3:
    case 4:
        c1 = 0x80808080;
        c0 = c1;
        break;
    default:
        c1 = 0x40808080;
        c0 = c1;
        break;
    }
    func_00207D00(1, 0);
    func_00207E40(1, 0x7C80, 0x7EA0, 0x100, 0x80, c0, 0x20046B05E1321D00ULL);
    if (D_00810CA4 == 2) {
        func_00207E40(1, 0x7480, 0x7EA0, 0x80, 0x80, c1, 0x20047E85DD322140ULL);
        func_00207E40(1, 0x7C80, 0x7EA0, 0x100, 0x80, c1, 0x20047705E1322040ULL);
        return;
    }
    if (D_00810CA4 == 0) {
        if (p[0x11] == 6) {
            func_00207E40(1, 0x7580, 0x7EA0, 0x100, 0x80, c0, 0x20047C85E1322000ULL);
            func_00212F30(D_00810CA6, c1);
            return;
        }
        func_00207E40(1, 0x7580, 0x7EA0, 0x100, 0x80, c1, 0x20047C85E1322000ULL);
        func_00212F30(D_00810CA6, c0);
        return;
    }
    func_00207E40(1, 0x7580, 0x7EA0, 0x80, 0x40, c0, 0x20046D059D322260ULL);
    if (D_00810CA4 == 1) {
        if (p[0x11] == 6) {
            func_00207E40(1, 0x7C80, 0x7EA0, 0x80, 0x40, c0, 0x20047E059D3222A0ULL);
            func_00212F30(D_00810CA6, c1);
            return;
        }
        func_00207E40(1, 0x7C80, 0x7EA0, 0x80, 0x40, c1, 0x20047E059D3222A0ULL);
        func_00212F30(D_00810CA6, c0);
        return;
    }
    if (p[0x11] == 2) {
        func_00212F30(D_00810CA5, c1);
    } else {
        func_00212F30(D_00810CA5, c0);
    }
    if (p[0x11] == 6) {
        func_00212F30(D_00810CA6, c1);
    } else {
        func_00212F30(D_00810CA6, c0);
    }
    if (p[0x11] == 5) {
        func_00212F30(D_00810CA7, c1);
        return;
    }
    func_00212F30(D_00810CA7, c0);
}
