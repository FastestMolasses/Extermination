// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00824390 (splat/link name 00824350; overlay code
//  is linked 0x40 below where it runs), 0x18C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Covers the splat pieces 00824350, 00824390 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: [44]'s step 2 (0x824390). +5 0: +5 = 1 and a 490-frame countdown in
//  +0x2A; +5 1: at zero calls 0x8249F0, then starts script 0x82B3D0 (0x8240E0
//  set) or 0x82B810 (0x824060 set) with +5 = 2, +6 = 1, or otherwise script
//  0x82BC90 with D_008107F4 += 1, the owner's (+0x1C) +0x2A += 1, +5 = +6 =
//  0; +0x2A = 0. +5 2: when the script ends the owner's +0x2A += 1, +5 = 0,
//  D_008107F4 += 1. Then func_001C6380, func_001B17A0 and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
extern char D_overlay_AREA13_0082B3D0[];
extern char D_overlay_AREA13_0082B810[];
extern char D_overlay_AREA13_0082BC90[];
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_overlay_AREA13_008249F0(unsigned char *self);
extern int func_overlay_AREA13_008240E0(void);
extern int func_overlay_AREA13_00824060(void);

void func_overlay_AREA13_00824350(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    unsigned char *parent = *(unsigned char **)(self + 0x1C);
    switch (self[5]) {
    case 0:
        self[5] = 1;
        *(short *)(self + 0x2A) = 0x1EA;
        break;
    case 1:
        *(short *)(self + 0x2A) -= 1;
        if (*(short *)(self + 0x2A) <= 0) {
            func_overlay_AREA13_008249F0(self);
            if (func_overlay_AREA13_008240E0() != 0) {
                func_001BA1A0(talk, D_overlay_AREA13_0082B3D0);
                self[5] = 2;
                self[6] = 1;
            } else if (func_overlay_AREA13_00824060() != 0) {
                func_001BA1A0(talk, D_overlay_AREA13_0082B810);
                self[5] = 2;
                self[6] = 1;
            } else {
                func_001BA1A0(talk, D_overlay_AREA13_0082BC90);
                *(unsigned char *)0x8107F4 += 1;
                *(short *)(parent + 0x2A) += 1;
                self[5] = 0;
                self[6] = 0;
            }
            *(short *)(self + 0x2A) = 0;
        }
        break;
    case 2:
        if (func_001BA1F0(self) != 0) {
            *(short *)(parent + 0x2A) += 1;
            self[5] = 0;
            *(unsigned char *)0x8107F4 += 1;
        }
        break;
    }
    func_001C6380(self);
    func_001B17A0(self);
    (*(ActorFn *)(self + 0x4C))(self);
}
