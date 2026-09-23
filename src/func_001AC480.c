// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// MATCH NOTE (m2-matching lane, 97.33% -> 100.0%): the scratchpad pointer at
// 0x70003B6C is a relocated extern (idiom-32), over-declared as an array
// because -sdatathreshold 8 would otherwise make it gp-relative (idiom #20).
// The literal spelling let mwcc speculate the scratchpad-base lui (0x7000) into two slots the
// target leaves as nop. The slot-byte decrement is `(*pf)--`, which reloads the
// byte after the test as the target does; `*pf = *pf - 1` reused the tested value.
//
// Semantics: state machine for the entity at *0x70003B6C, on its state byte
// +9: 0 = init (func_00119828 x2, count +0x16 = 0x4B0, slot +0xF from
// D_00275BDC, D_00275BD8 = 1, func_001FF080(0, 1)); 1 = advance once
// D_00275BD8 is clear and func_001FB370 says so (the target loads
// D_0028A4A8[0] into $a0 for that call although the callee reads none);
// 2 = count down or react to the D_00810E74 buttons 0x840 (confirm, SFX
// 0x5DD + slot), 0x4000 / 0x1000 (slot up / down, SFX 5); 3 and 4 return 1 or
// 3 when D_0028A9A0[0] == 2, and 4 re-enters 2 on D_00810E70[0].

extern unsigned char *D_70003B6C[4];                   /* PS2 scratchpad @ 0x70003B6C */

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

    p = D_70003B6C[0];
    pp = p + 9;
    state = p[9];
    switch (state) {
    case 0:
        func_00119828(0, 0x5998, 0x5998);
        func_00119828(1, 0x5998, 0x5998);
        p = D_70003B6C[0];
        p[9] = p[9] + 1;
        ((unsigned short *)D_70003B6C[0])[0xB] = 0x4B0;
        if (D_00275BDC == 0) {
            (D_70003B6C[0])[0xF] = 0;
        } else {
            (D_70003B6C[0])[0xF] = 1;
        }
        D_00275BD8 = 1;
        func_001FF080(0, 1);
        break;
    case 1:
        if (D_00275BD8 == 0 && func_001FB370(D_0028A4A8[0]) != 0) {
            p = D_70003B6C[0];
            p[9] = p[9] + 1;
            func_001AEE10(4, 0);
        }
        break;
    case 2:
        func_001AC7F0();
        if (D_0028A9A0[0] == 0) {
            if (D_00810E70[0] == 0) {
                unsigned short *q = (unsigned short *)D_70003B6C[0];
                unsigned short cnt = q[0xB];
                q[0xB] = cnt - 1;
                if (cnt == 0) {
                    func_001AEDE0(4, 0);
                    (D_70003B6C[0])[9] = 4;
                }
            } else {
                if (D_00810E74[0] & 0x840) {
                    unsigned char slot;
                    func_001AEDE0(4, 0);
                    (D_70003B6C[0])[9] = 3;
                    slot = (D_70003B6C[0])[0xF];
                    if (slot == 0) {
                        func_001FB9F0(0x5DD, 0x1000, 0x1000, 0x1000);
                    } else if (slot == 1) {
                        func_001FB9F0(0x5DE, 0x1000, 0x1000, 0x1000);
                    } else {
                        func_001FB9F0(0x5DF, 0x1000, 0x1000, 0x1000);
                    }
                } else if (D_00810E74[0] & 0x4000) {
                    if ((int)(D_70003B6C[0])[0xF] < 2) {
                        func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                        p = D_70003B6C[0];
                        p[0xF] = p[0xF] + 1;
                    }
                } else if (D_00810E74[0] & 0x1000) {
                    unsigned char *pf = D_70003B6C[0] + 0xF;
                    if (*pf != 0) {
                        --*pf;
                        func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                    }
                }
                ((unsigned short *)D_70003B6C[0])[0xB] = 0x4B0;
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
            ((char *)D_70003B6C[0])[0xA] = 0;
            (D_70003B6C[0])[9] = 2;
            ((unsigned short *)D_70003B6C[0])[0xB] = 0x4B0;
        }
        break;
    }
    return 0;
}
