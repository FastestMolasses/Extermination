// NEARMISS func_001AC480  (vram 0x001AC480, 0x370 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.33% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single delay-slot/scheduling residual around the D_00275BDC branch in state 0 (one nop/lui shifted by 4 bytes), otherwise byte-exact. Body/control-flow fully recovered: descending-order 5-state switch (idiom-20) dispatched on the entity state byte at *0x70003B6C+9, gp-rel D_00275BD8/D_00275BDC at...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern void func_00119828(int a, int b, int c);
extern void func_001AC7F0(void);
extern void func_001AEDE0(int a, int b);
extern void func_001AEE10(int a, int b);
extern int func_001FB370(int p);
extern void func_001FB9F0(int a, int b, int c, int d);
extern void func_001FF080(int a, int b);
extern unsigned char D_00275BD8;
extern unsigned char D_00275BDC;
extern int D_0028A4A8[4];
extern short D_0028A9A0[8];
extern unsigned short D_00810E70[8];
extern unsigned short D_00810E74[8];

int func_001AC480(void) {
    unsigned char *p;
    unsigned char *pp;
    int state;

    p = *(unsigned char **)0x70003B6C;
    pp = p + 9;
    state = p[9];
    switch (state) {
    case 0:
        func_00119828(0, 0x5998, 0x5998);
        func_00119828(1, 0x5998, 0x5998);
        p = *(unsigned char **)0x70003B6C;
        p[9] = p[9] + 1;
        (*(unsigned short **)0x70003B6C)[0xB] = 0x4B0;
        if (D_00275BDC == 0) {
            (*(unsigned char **)0x70003B6C)[0xF] = 0;
        } else {
            (*(unsigned char **)0x70003B6C)[0xF] = 1;
        }
        D_00275BD8 = 1;
        func_001FF080(0, 1);
        break;
    case 1:
        if (D_00275BD8 == 0 && func_001FB370(D_0028A4A8[0]) != 0) {
            p = *(unsigned char **)0x70003B6C;
            p[9] = p[9] + 1;
            func_001AEE10(4, 0);
        }
        break;
    case 2:
        func_001AC7F0();
        if (D_0028A9A0[0] == 0) {
            if (D_00810E70[0] == 0) {
                unsigned short *q = *(unsigned short **)0x70003B6C;
                unsigned short cnt = q[0xB];
                q[0xB] = cnt - 1;
                if (cnt == 0) {
                    func_001AEDE0(4, 0);
                    (*(unsigned char **)0x70003B6C)[9] = 4;
                }
            } else {
                if (D_00810E74[0] & 0x840) {
                    unsigned char slot;
                    func_001AEDE0(4, 0);
                    (*(unsigned char **)0x70003B6C)[9] = 3;
                    slot = (*(unsigned char **)0x70003B6C)[0xF];
                    if (slot == 0) {
                        func_001FB9F0(0x5DD, 0x1000, 0x1000, 0x1000);
                    } else if (slot == 1) {
                        func_001FB9F0(0x5DE, 0x1000, 0x1000, 0x1000);
                    } else {
                        func_001FB9F0(0x5DF, 0x1000, 0x1000, 0x1000);
                    }
                } else if (D_00810E74[0] & 0x4000) {
                    if ((int)(*(unsigned char **)0x70003B6C)[0xF] < 2) {
                        func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                        p = *(unsigned char **)0x70003B6C;
                        p[0xF] = p[0xF] + 1;
                    }
                } else if (D_00810E74[0] & 0x1000) {
                    unsigned char *pf = *(unsigned char **)0x70003B6C + 0xF;
                    if (*pf != 0) {
                        *pf = *pf - 1;
                        func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                    }
                }
                (*(unsigned short **)0x70003B6C)[0xB] = 0x4B0;
            }
        }
        break;
    case 3:
        func_001AC7F0();
        if (D_0028A9A0[0] == 2) {
            return 1;
        }
        break;
    case 4:
        func_001AC7F0();
        if (D_0028A9A0[0] == 2) {
            return 3;
        }
        if (D_00810E70[0] != 0) {
            func_001AEE10(4, 0);
            (*(char **)0x70003B6C)[0xA] = 0;
            (*(unsigned char **)0x70003B6C)[9] = 2;
            (*(unsigned short **)0x70003B6C)[0xB] = 0x4B0;
        }
        break;
    }
    return 0;
}
