// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00825FC0 (splat/link name 00825F80; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: sub-state +5 0 starts script 0x82B080 when D_00810803 is 0x80;
//  +5 1 counts +0x28 (func_001AEDE0(1, 1) at 390) and at script end calls
//  func_001FB0B0(0) and sets state 3. Animates unless D_00810803 is 0xFF.
//  Called from 0x825D70.
// Covers the splat pieces 00825F80, 00825FC0 (the later piece is absorbed at
// link time, tools/overlay/fill_overlay.py).
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810803;
extern char D_overlay_AREA00_0082B080[];
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern void func_001AEDE0(int a, int b);
extern void func_001FB0B0(int a0);
extern void func_001C6380(unsigned char *self);
extern void func_001B1B70(unsigned char *self);

void func_overlay_AREA00_00825F80(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (D_00810803 == 0x80) {
            self[5] = 1;
            func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_0082B080);
            *(short *)(self + 0x28) = 0;
        }
        break;
    case 1:
        (*(short *)(self + 0x28))++;
        if (*(short *)(self + 0x28) == 0x186) {
            func_001AEDE0(1, 1);
        }
        if (func_001BA1F0(self) != 0) {
            func_001FB0B0(0);
            self[4] = 3;
        }
        break;
    }
    if (D_00810803 != 0xFF) {
        func_001C6380(self);
        self[1] = 1;
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
    }
}
