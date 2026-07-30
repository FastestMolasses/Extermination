// NEARMISS func_00115E50  (vram 0x00115E50, 0x744 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 63.70% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc register-allocation wall (parameter spill decision). Logic is fully recovered: the three-term bail test with its bnel/blezl eager-annul forms, the 0xFF / &0x80 / plain fan-out that sets ev[0x2A]/ev[0x2C], the voice loop with strength-reduced i*16 offset, the whole 0x6A-byte voice record, th...
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

struct tone {
    int t00, t04, t08;
};

extern unsigned char D_00281AC0[];
extern struct voice D_0027CCC0[];
extern unsigned char D_0027F740[];
extern struct tone D_0027C6C0[];
extern unsigned short D_00242630[];

extern void func_001177E8(void);
extern int func_001178C0(int a, int b, int c);
extern int func_001172B8(int a);
extern int func_00117BA0(void);
extern int func_00117918(int a, int b, int c, int d, int e);
extern void func_001157F0(int a, int b, int c, int d);
extern int func_001179E0(int a, unsigned char *b);

int func_00115E50(unsigned char *ev) {
    unsigned char **snd = (unsigned char **)D_00281AC0;
    int inv = 0;
    int i;
    int key;
    int t;
    long mask;
    int ret;

    if (ev[3] == 0) {
        func_001177E8();
        return -1;
    }

    if (snd[3][3] == 0 ||
        (D_0027C6C0[*(unsigned short *)(ev + 0x24)].t08 & 1) != 0 ||
        snd[0][6] - ev[2] > 0) {
        ret = -1;
        goto done;
    }

    if (snd[0][0] == 0xFF) {
        t = ev[2] - snd[0][6];
        *(unsigned short *)(ev + 0x2A) = t;
        *(unsigned short *)(ev + 0x2C) = t;
    } else if (snd[0][0] & 0x80) {
        inv = 1;
        *(unsigned short *)(ev + 0x2C) = snd[0][0] - 0x80;
        *(unsigned short *)(ev + 0x2A) = 0;
    } else {
        *(unsigned short *)(ev + 0x2C) = snd[0][0];
        *(unsigned short *)(ev + 0x2A) = 0;
    }

    for (i = *(unsigned short *)(ev + 0x2A); i < *(unsigned short *)(ev + 0x2C) + 1; i++) {
        if (func_001178C0(snd[0][0], i, ev[2]) != 1) {
            continue;
        }
        key = func_001172B8(i);
        if (key == -1) {
            break;
        }

        mask = (long)1 << key;
        snd[1] += i * 16;
        if (snd[1][0xF] & 1) {
            D_0027CCC0[key].f08 = 0;
            D_0027CCC0[key].f0C = 1;
        } else {
            D_0027CCC0[key].f08 = 0;
            D_0027CCC0[key].f0C = 0;
        }

        D_0027CCC0[key].f00 = 1;
        D_0027CCC0[key].f02 = ev[2];
        D_0027CCC0[key].f04 = ev[0] & 0xF;
        D_0027CCC0[key].f06 = *(unsigned short *)(ev + 0x18);
        D_0027CCC0[key].f0A = *(unsigned short *)(D_0027F740 + 0x34);
        D_0027CCC0[key].f0E = i;
        D_0027CCC0[key].f12 = 0;
        D_0027CCC0[key].f1A = 1;
        D_0027CCC0[key].f1C = 0;
        D_0027CCC0[key].f1E = snd[1][1];
        D_0027CCC0[key].f20 = snd[1][0];
        D_0027CCC0[key].f22 = *(unsigned short *)(ev + 0x24);
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
        D_0027CCC0[key].f4C = snd[3][4];
        D_0027CCC0[key].f4E = 0x78;

        if (snd[0][0] == 0xFF) {
            D_0027CCC0[key].f66 = 1;
            D_0027CCC0[key].f68 = 0x40;
        } else {
            D_0027CCC0[key].f66 = 0;
            D_0027CCC0[key].f68 = snd[0][7];
        }

        if (snd[3][0xB] == 0x7F) {
            D_0027CCC0[key].f18 = 1;
        }

        if ((snd[1][0xF] & 0x20) && snd[3][9] != 0) {
            if (snd[1][0xF] & 0x40) {
                D_0027CCC0[key].f10 = snd[0][5];
            } else {
                D_0027CCC0[key].f10 = snd[1][0xE];
            }
            D_0027CCC0[key].f14 = 1;
            D_0027CCC0[key].f16 = snd[3][9];
        } else {
            D_0027CCC0[key].f14 = 0;
            D_0027CCC0[key].f16 = 0;
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

        func_001157F0(3, key, *(unsigned short *)(snd[1] + 6),
                      *(unsigned short *)(snd[1] + 8));

        *(long *)(D_0027F740 + 0x20) |= mask;
        if (snd[1][0xF] & 0x80) {
            *(long *)(D_0027F740 + 0x00) |= mask;
        } else {
            *(long *)(D_0027F740 + 0x00) &= ~mask;
        }
        *(long *)(D_0027F740 + 0x10) &= ~mask;
        *(int *)(D_0027F740 + 0x34) += 1;
        snd[1] -= i * 16;

        if (inv == 0) {
            break;
        }
    }

    ret = 0;
done:
    *(int *)(ev + 8) += 3;
    return ret;
}
