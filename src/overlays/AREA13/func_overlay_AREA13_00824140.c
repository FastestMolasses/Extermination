// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00824180 (splat/link name 00824140; overlay code
//  is linked 0x40 below where it runs), 0x208 bytes.
// Byte-identical once linked (lane A13C): overlay_match.py reports 99.98
//  because the switch table is .rodata that needs pinning;
//  tools/overlay/jt_pin.py places the compiled table at the original address
//  0x82E200 at link.
// Covers the splat pieces 00824140, 00824160, 00824180 (the later pieces are
//  absorbed at link time, tools/overlay/fill_overlay.py).
// Role: [44]'s step 1 (from 0x823E90), a seven-way switch on +5 compiled to a
//  jump table (the original's table is at 0x82E200). 0: on +0xB bit 2, either
//  +5 += 1, +0xA = 1, +0 = 2 (with +0xB bit 0) or script 0x82B090 and +5 = 2;
//  1: script 0x82B2D0, then +5 = 3 (+0xA == 0) or +5 = 4 and D_00810774 = 1;
//  2: when the script ends D_008106B1 = +0x34 + 0x80, D_008106B0 = 1,
//  D_008106D0 = +0x14, +0xA = +0xB = 0, +0 = 1, +5 = 1; 3: when it ends +0xB
//  = 0, +0 = 1, +5 = 0; 4: +5 += 1 when it ends; 5: D_008107F4 += 1, +5 = 0;
//  6: nothing. Every frame func_001026A0(scratch 0x700038B0, +0xD0, (-1.333,
//  1.9, -2.6, 1)), func_001F5940(4, 0x700038B0, 0), func_001C6380,
//  func_001B17A0 and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
extern float D_700038A0[4];
extern float D_700038B0[4];
extern char D_overlay_AREA13_0082B090[];
extern char D_overlay_AREA13_0082B2D0[];
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001F5940(int kind, void *pos, int a2);
extern void func_001C6380(unsigned char *self);
extern void func_001B17A0(unsigned char *self);

void func_overlay_AREA13_00824140(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    unsigned char b;
    switch (self[5]) {
    case 0:
        b = self[0xB];
        if (b & 4) {
            if (b & 1) {
                self[5]++;
                self[0xA] = 1;
                self[0] = 2;
            } else {
                func_001BA1A0(talk, D_overlay_AREA13_0082B090);
                self[5] = 2;
            }
        }
        break;
    case 1:
        func_001BA1A0(talk, D_overlay_AREA13_0082B2D0);
        if (self[0xA] == 0) {
            self[5] = 3;
        } else {
            self[5] = 4;
            *(unsigned char *)0x810774 = 1;
        }
        break;
    case 2:
        if (func_001BA1F0(self) != 0) {
            *(unsigned char *)0x8106B1 = self[0x34] + 0x80;
            *(unsigned char *)0x8106B0 = 1;
            *(int *)0x8106D0 = *(int *)(self + 0x14);
            self[0xA] = 0;
            self[0xB] = 0;
            self[0] = 1;
            self[5] = 1;
        }
        break;
    case 3:
        if (func_001BA1F0(self) != 0) {
            self[0xB] = 0;
            self[0] = 1;
            self[5] = 0;
        }
        break;
    case 4:
        if (func_001BA1F0(self) != 0) {
            self[5]++;
        }
        break;
    case 5:
        *(unsigned char *)0x8107F4 += 1;
        self[5] = 0;
        break;
    case 6:
        break;
    }
    *(float *)0x700038AC = 1.0f;
    *(float *)0x700038A0 = -1.333f;
    *(float *)0x700038A4 = 1.9f;
    *(float *)0x700038A8 = -2.6f;
    func_001026A0(D_700038B0, self + 0xD0, D_700038A0);
    func_001F5940(4, D_700038B0, 0);
    func_001C6380(self);
    func_001B17A0(self);
    (*(ActorFn *)(self + 0x4C))(self);
}
