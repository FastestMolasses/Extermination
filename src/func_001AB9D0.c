// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// SEMANTICS: 6-state sequencer driven off the global object pointer held in
// scratchpad word 0x70003B6C. Byte field 9 of that object is the state index
// (jump-table dispatch, 6 entries); byte field 0xA is the state-3 sub-state,
// halfword 0x16 a countdown timer, byte 0xF a flag. Each state advances the
// state byte by 1 after re-reading the global pointer. States 4 and 5 emit the
// same four 64-bit GS/DMA packet words via func_001ABF90. Returns 1 only from
// state 5 when D_0028A9A0 == 2 (sequence finished), 0 otherwise.

extern unsigned char D_00275BD8;
extern short D_0028A9A0[8];

extern int func_001AEDB0(int, unsigned char *);
extern int func_001AEDE0(int, int);
extern int func_001AEE10(int, int);
extern int func_001FF080(int, int);
extern int func_001D1C50(unsigned char *, unsigned char *);
extern int func_001D1EA0(int);
extern int func_001D1EF0(unsigned char *, unsigned char *);
extern int func_0022A460(int);
extern int func_001ABF90(long long, long long, long long, long long);

int func_001AB9D0(void) {
    unsigned char *p;
    unsigned char *q;
    unsigned char *s;
    unsigned short t;

    p = *(unsigned char **)0x70003B6C;
    s = p + 9;
    switch (p[9]) {
    case 0:
        *s = *s + 1;
        func_001AEDB0(0, s);
        break;
    case 1:
        D_00275BD8 = 1;
        func_001FF080(0, 0x28);
        q = *(unsigned char **)0x70003B6C;
        q[9] = q[9] + 1;
        break;
    case 2:
        if (D_00275BD8 == 0) {
            func_001AEE10(4, 0);
            q = *(unsigned char **)0x70003B6C;
            q[9] = q[9] + 1;
        }
        break;
    case 3:
        switch (p[0xA]) {
        case 0:
            func_001D1C50(p, s);
            if (func_0022A460(0) != 0) {
                func_001AEDE0(8, 0);
                q = *(unsigned char **)0x70003B6C;
                q[0xA] = q[0xA] + 1;
            }
            func_001D1EA0(0);
            break;
        case 1:
            func_001D1EF0(p, s);
            if (D_0028A9A0[0] == 2) {
                q = *(unsigned char **)0x70003B6C;
                q[9] = q[9] + 1;
                *(unsigned short *)(*(unsigned char **)0x70003B6C + 0x16) = 0x12C;
                (*(unsigned char **)0x70003B6C)[0xF] = 0;
                (*(unsigned char **)0x70003B6C)[0xA] = 0;
                func_001AEE10(4, 0);
            }
            break;
        }
        break;
    case 4:
        func_001ABF90(0x2005C40621322A00LL, 0x2005C48621322A40LL, 0x2005C60621322C00LL, 0x2005C68621322C40LL);
        q = *(unsigned char **)0x70003B6C;
        t = *(unsigned short *)(q + 0x16);
        *(unsigned short *)(q + 0x16) = t - 1;
        if (t == 0) {
            func_001AEDE0(4, 0);
            q = *(unsigned char **)0x70003B6C;
            q[9] = q[9] + 1;
        }
        break;
    case 5:
        func_001ABF90(0x2005C40621322A00LL, 0x2005C48621322A40LL, 0x2005C60621322C00LL, 0x2005C68621322C40LL);
        if (D_0028A9A0[0] == 2) {
            return 1;
        }
        break;
    }
    return 0;
}
