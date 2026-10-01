// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00824520 (splat/link name 008244E0; overlay code
//  is linked 0x40 below where it runs), 0x1AC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Covers the splat pieces 008244E0, 00824520 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: [44]'s step 3 (0x824520): three scripts released by the owner's
//  (+0x1C) short +0x28. +5 0 (count >= 2): script 0x82BD10 when func_001BA1F0
//  is set (+6 0) or at once (+6 1), +5 = 1; +5 1 (count >= 3): script
//  0x82BDD0, +5 = 2; +5 2 (count >= 4): script 0x82BE90, +5 = 3; +5 3: when
//  it ends func_001FABB0(), func_001FA790(0, 0x12), D_008107F4 += 1, +5 = 0.
//  Then func_001C6380, func_001B17A0 and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
extern char D_overlay_AREA13_0082BD10[];
extern char D_overlay_AREA13_0082BDD0[];
extern char D_overlay_AREA13_0082BE90[];
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FABB0(void);
extern void func_001FA790(int a, int b);
extern void func_001C6380(unsigned char *self);
extern void func_001B17A0(unsigned char *self);

void func_overlay_AREA13_008244E0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    unsigned char *parent = *(unsigned char **)(self + 0x1C);
    switch (self[5]) {
    case 0:
        if (*(short *)(parent + 0x28) >= 2) {
            switch (self[6]) {
            case 0:
                if (func_001BA1F0(self) != 0) {
                    func_001BA1A0(talk, D_overlay_AREA13_0082BD10);
                    self[5] = 1;
                }
                break;
            case 1:
                func_001BA1A0(talk, D_overlay_AREA13_0082BD10);
                self[5] = 1;
                break;
            }
        }
        break;
    case 1:
        if (*(short *)(parent + 0x28) >= 3) {
            func_001BA1A0(talk, D_overlay_AREA13_0082BDD0);
            self[5] = 2;
        }
        func_001BA1F0(self);
        break;
    case 2:
        if (*(short *)(parent + 0x28) >= 4) {
            func_001BA1A0(talk, D_overlay_AREA13_0082BE90);
            self[5] = 3;
        }
        func_001BA1F0(self);
        break;
    case 3:
        if (func_001BA1F0(self) != 0) {
            func_001FABB0();
            func_001FA790(0, 0x12);
            *(unsigned char *)0x8107F4 += 1;
            self[5] = 0;
        }
        break;
    }
    func_001C6380(self);
    func_001B17A0(self);
    (*(ActorFn *)(self + 0x4C))(self);
}
