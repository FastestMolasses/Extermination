// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Weapon/HUD reticle selector. Calls func_001FC280, then reads two globals:
// D_008106C8 (bits 8..15 -> s0, masked to 0x80 and OR'd with the current weapon
// id D_00810D38 when that is nonzero). If the screen-state byte D_00810700 is
// 0x15, or the weapon id is 0xB/0xC/0x17, or the mode flag D_008104E4 != 1,
// it takes the dispatch path: queries func_00122BB8 for a reticle index
// (s2 = (ret>>16)&0x7F), then per arg0 (force flag) and the masked weapon id
// s0&0x7F either spawns/refreshes the reticle via func_001FAB50/func_001FABF0
// (slot s0, sprite s2+0x10E) or, in the special-state branch, kicks
// func_001FAAC0 + a fixed func_001FABF0(0,0x18,0x40,1).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 89.3%. Key: write the (s0 & 0x80) | id update as two in-place statements
// (s0 &= 0x80; s0 |= id;) so the AND lands back in s0 (matching the target's
// `andi s0,s0,0x80; or s0,s0,a1` register reuse) rather than a temp.
extern int func_00122BB8(unsigned char a, int b);
extern void func_001FAAC0(int a, int b);
extern void func_001FAB50(void);
extern void func_001FABF0(int a, int b, int c, int d);
extern void func_001FC280(void);
extern signed char D_00282154;
extern int D_00282178;
extern unsigned char D_008104E4;
extern int D_008106C8;
extern unsigned char D_00810700;
extern int D_00810D38;

void func_001FAE70(int arg0) {
    int s0;
    int s2;
    unsigned char a0;

    func_001FC280();
    s0 = (int)(D_008106C8 & 0xFF00) >> 8;
    if (D_00810D38 != 0) {
        s0 &= 0x80;
        s0 |= D_00810D38;
    }
    a0 = D_00810700;
    if ((a0 != 0x15) && (D_00810D38 != 0xB) && (D_00810D38 != 0xC) &&
        (D_00810D38 != 0x17) && (a0 = D_008104E4, a0 == 1)) {
        if (D_00282178 != 0x18) {
            func_001FAAC0(0, D_00810D38);
            func_001FABF0(0, 0x18, 0x40, 1);
        }
        return;
    }
    s2 = (func_00122BB8(a0, D_00810D38) >> 0x10) & 0x7F;
    if (arg0 != 0) {
        func_001FAB50();
        s0 = s0 & 0x7F;
        if (s0 != 0) {
            func_001FABF0(0, s0, s2 + 0x10E, 1);
        }
        return;
    }
    s0 = s0 & 0x7F;
    if (s0 == 0) {
        func_001FAB50();
        return;
    }
    if ((D_00282154 == 0) || (D_00282178 != s0)) {
        func_001FAB50();
        func_001FABF0(0, s0, s2 + 0x10E, 1);
    }
}
