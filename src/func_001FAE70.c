// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Music stream-resume cue selector (FINDINGS "Public API": func_001FB0B0(cue)
// writes the current-BGM global D_00810D38 and tail-calls func_001FAE70(1)).
// Calls func_001FC280 first. The cue s0 = bits 8..15 of the per-area flags word
// D_008106C8; when D_00810D38 is nonzero, s0 = (s0 & 0x80) | D_00810D38.
// OVERRIDE: when area byte D_00810700 != 0x15, D_00810D38 is not 0xB/0xC/0x17
// and D_008104E4 == 1, it plays cue 0x18 on music channel 0 (func_001FAAC0(0)
// release + func_001FABF0(0,0x18,0x40,1)) unless channel 0 already holds cue
// 0x18 (D_00282178). Otherwise the fade length is s2+0x10E = 270 + seven bits
// of the LCG func_00122BB8 ((ret>>16)&0x7F). arg0 != 0: release channel 0
// (func_001FAB50) and start cue s0&0x7F with that fade when nonzero. arg0 == 0
// (resume): cue 0 releases the channel; otherwise it restarts only when
// channel 0 is idle (D_00282154 == 0) or holds a different cue. The earlier
// "weapon/HUD reticle selector" label was wrong: func_001FABF0(ch, cue, fade,
// 1) starts the channel's stream via func_001FA790 and sets the fade step
// 16383/fade; it draws nothing.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 89.3%. Key: write the (s0 & 0x80) | id update as two in-place statements
// (s0 &= 0x80; s0 |= id;) so the AND lands back in s0 (matching the target's
// `andi s0,s0,0x80; or s0,s0,a1` register reuse) rather than a temp.
extern int func_00122BB8(void);
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
    s2 = (func_00122BB8() >> 0x10) & 0x7F;
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
