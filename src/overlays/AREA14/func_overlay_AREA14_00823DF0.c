// NEARMISS func_overlay_AREA14_00823DF0 (95.96%, mwcc 2.3.3, 0x10 longer; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA14 overlay, runtime 0x00823E30 (splat/link name 00823DF0; overlay code
//  is linked 0x40 below where it runs), 0x18C bytes.
// Role: sub 0 placement [4]. State 0: state 3 when func_001BA1C0(self,
//  0x3F) is set or D_00810C8C is 0, else state 1 and +0 = 1. State 1, +5 0:
//  when D_00810702 == 1: +0x28 = 360, script 0x827490, +5 1. +5 1: while
//  +0x28 > 0: func_001BA1F0(self) (result unused), func_00183160(2, 0.0),
//  +0x28 - 1; at 0: func_00183160(0, 0.0), script 0x827590, +5 2. +5 2: at
//  the script end state 3, func_001C4760(0x18, 1), D_00810817 = 0xFF. States
//  2 / 3: func_001AFC10.
// Divergence: the original fills the dispatch and test delay slots by
//  moving the successor's first instruction (the 0x3F argument, the
//  0x810C8C / 0x810702 address halves) and leaves no copy at the label;
//  mwcc 2.3.3 fills the same slots but re-emits each filler dead at the
//  label (idiom-13b), four extra words. The 991202 build drops the dead
//  copies but then fills the three jal slots differently (87.88). Literal
//  addresses for the three globals raised it from 94.75.
#define S16(o) (*(short *)(self + (o)))
extern char D_overlay_AREA14_00827490[];
extern char D_overlay_AREA14_00827590[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_00183160(int mode, float f);
extern void func_001C4760(int id, int on);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA14_00823DF0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x3F) != 0) {
            self[4] = 3;
            break;
        }
        if (*(unsigned char *)0x810C8C == 0) {
            self[4] = 3;
            break;
        }
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (*(unsigned char *)0x810702 == 1) {
                S16(0x28) = 360;
                func_001BA1A0(talk, D_overlay_AREA14_00827490);
                self[5] = 1;
            }
            break;
        case 1:
            if (S16(0x28) > 0) {
                func_001BA1F0(self);
                func_00183160(2, 0.0f);
                S16(0x28)--;
            } else {
                func_00183160(0, 0.0f);
                func_001BA1A0(talk, D_overlay_AREA14_00827590);
                self[5] = 2;
            }
            break;
        case 2:
            if (func_001BA1F0(self) != 0) {
                self[4] = 3;
                func_001C4760(0x18, 1);
                *(unsigned char *)0x810817 = 0xFF;
            }
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
