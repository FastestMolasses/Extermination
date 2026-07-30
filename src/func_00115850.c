// NEARMISS func_00115850  (vram 0x00115850, 0x5FC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 68.60% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc register-allocation / spill-placement wall. Instruction stream and control flow are fully recovered (all ~40 voice-record stores, the mult/beql-break/div 44100/48000 idiom, the sq/lq t9 save across the call, the 64-bit sd/ld mask ops and the dsllv all reproduce). Residual is pure GPR colori...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

struct voice {
    unsigned short f00, f02, f04, f06, f08, f0A, f0C, f0E;
    unsigned short f10, f12, f14, f16, f18, f1A, f1C, f1E;
    unsigned short f20, f22, f24, f26, f28, f2A, f2C, f2E;
    unsigned short f30, f32, f34, f36, f38, f3A, f3C, f3E;
    unsigned short f40, f42, f44, f46, f48, f4A, f4C, f4E;
    unsigned short f50, f52, f54, f56, f58, f5A, f5C, f5E;
    unsigned short f60, f62, f64, f66, f68;
};

struct tone { int t00; int t04; int t08; };

extern unsigned char D_00281AC0[];
extern struct voice D_0027CCC0[];
extern unsigned char D_0027F740[];
extern struct tone D_0027C6C0[];
extern unsigned short D_00242630[];

extern void func_001176E0(void);
extern int func_00117428(int a, int b, int c);
extern int func_00117BA0(void);
extern int func_00117918(int a, int b, int c, int d, int e);
extern void func_001157F0(int a, int b, int c, int d);
extern int func_001179E0(int a, unsigned char *b);

int func_00115850(unsigned char *ev) {
    unsigned char **snd = (unsigned char **)D_00281AC0;
    int slot;
    int key;
    int t;
    long mask;

    if (ev[3] == 0) {
        func_001176E0();
        return 0;
    }

    slot = ev[2] - snd[0][6];
    if (slot < 0) {
        *(int *)(ev + 8) += 4;
        return 0;
    }

    snd[1] += slot * 16;
    snd[3][0xA] = 0x40;
    key = func_00117428(snd[1][0], snd[1][1], *(unsigned short *)(ev + 0x24));
    if (key == -1) {
        *(int *)(ev + 8) += 4;
        return 0;
    }

    mask = (long)1 << key;

    if (snd[1][0xF] & 1) {
        D_0027CCC0[key].f08 = 0;
        D_0027CCC0[key].f0C = 1;
    } else {
        D_0027CCC0[key].f08 = 1;
        D_0027CCC0[key].f0C = 0;
    }

    D_0027CCC0[key].f00 = 1;
    D_0027CCC0[key].f02 = ev[2];
    D_0027CCC0[key].f04 = ev[0] & 0xF;
    D_0027CCC0[key].f06 = *(unsigned short *)(ev + 0x18);
    D_0027CCC0[key].f0A = *(unsigned short *)(D_0027F740 + 0x34);
    D_0027CCC0[key].f0E = slot;
    D_0027CCC0[key].f12 = 0;
    D_0027CCC0[key].f18 = 0;
    D_0027CCC0[key].f1A = 2;
    D_0027CCC0[key].f1C = 0;
    D_0027CCC0[key].f1E = snd[1][1];
    D_0027CCC0[key].f20 = snd[1][0];
    D_0027CCC0[key].f22 = *(unsigned short *)(ev + 0x24);
    D_0027CCC0[key].f24 = *(unsigned short *)(ev + 0x28);
    D_0027CCC0[key].f26 = *(unsigned short *)(ev + 0x26);
    D_0027CCC0[key].f28 = snd[3][0xE];
    D_0027CCC0[key].f2A = snd[0][1];
    D_0027CCC0[key].f2C = snd[6][ev[3] + 2];
    D_0027CCC0[key].f2E = snd[1][0xB];
    D_0027CCC0[key].f30 = snd[2][0];
    t = func_00117BA0();
    D_0027CCC0[key].f32 = D_00242630[t >> 2];
    D_0027CCC0[key].f34 = snd[3][3];
    D_0027CCC0[key].f36 = (signed char)snd[1][3];
    D_0027CCC0[key].f38 = snd[3][0xA];
    D_0027CCC0[key].f3A = snd[1][0xD];
    D_0027CCC0[key].f3C = snd[3][0xC];
    D_0027CCC0[key].f3E = ev[4];
    D_0027CCC0[key].f40 = snd[1][2];
    D_0027CCC0[key].f42 = 0;
    D_0027CCC0[key].f44 = 0;
    D_0027CCC0[key].f4A = snd[1][0xA];
    D_0027CCC0[key].f4C = snd[1][0xC];
    D_0027CCC0[key].f4E = 0x78;
    D_0027CCC0[key].f62 = 0;
    D_0027CCC0[key].f66 = 0;
    D_0027CCC0[key].f68 = 0;

    if (snd[1][0xF] & 0x20) {
        if (snd[1][0xF] & 0x40) {
            D_0027CCC0[key].f10 = snd[0][5];
        } else {
            D_0027CCC0[key].f10 = snd[1][0xE];
        }
        D_0027CCC0[key].f14 = 1;
        D_0027CCC0[key].f16 = 0x7F;
    } else {
        D_0027CCC0[key].f14 = 0;
    }

    if (snd[1][0xF] & 0x10) {
        t = snd[0][4];
    } else {
        t = snd[1][0xD];
    }
    t = func_00117918(snd[1][2], ev[2], *(signed char *)(snd[1] + 3), snd[3][0xA], t);
    func_001157F0(6, key, t * 44100 / 48000, 0);

    t = func_001179E0(key, ev);
    func_001157F0(1, key, t >> 16, t & 0xFFFF);

    func_001157F0(5, key,
                  (*(unsigned short *)(snd[1] + 4) +
                   D_0027C6C0[*(unsigned short *)(ev + 0x24)].t08)
                      << 3,
                  0);

    func_001157F0(3, key, *(unsigned short *)(snd[1] + 6), *(unsigned short *)(snd[1] + 8));

    *(long *)(D_0027F740 + 0x20) |= mask;
    if (*(unsigned short *)(ev + 0x42) == 1) {
        *(long *)(D_0027F740 + 0x00) |= mask;
    } else if (snd[1][0xF] & 0x80) {
        *(long *)(D_0027F740 + 0x00) |= mask;
    } else {
        *(long *)(D_0027F740 + 0x00) &= ~mask;
    }

    if (snd[1][0xF] & 2) {
        *(long *)(D_0027F740 + 0x10) |= mask;
        func_001157F0(0x33, 8, key, snd[1][2]);
    } else {
        *(long *)(D_0027F740 + 0x10) &= ~mask;
    }

    *(int *)(D_0027F740 + 0x34) += 1;
    snd[1] -= slot * 16;
    *(int *)(ev + 8) += 4;
    return 0;
}
