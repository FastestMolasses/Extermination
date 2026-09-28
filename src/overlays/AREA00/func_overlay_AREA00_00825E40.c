// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00825E80 (splat/link name 00825E40; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: sub-state +5 0 tests func_001B1EA0(0, D_00810350, quad, 4) with the
//  four-point area copied from 0x82B740 and starts script 0x82ABC0; +5 1
//  (D_00810803 == 2 sets +0x2E = 0xFFFF) at script end sets D_00810803 = 3,
//  D_0081028C = pi/2, calls func_001B6660(0x827F50) and func_001FB0B0(0x17).
//  Then animates. Called from 0x825D70.
// Covers the splat pieces 00825E40, 00825E80 (the later piece is absorbed at
// link time, tools/overlay/fill_overlay.py).
typedef void (*ActorFn)(unsigned char *);
typedef struct { float v[4][4]; } Quad __attribute__((aligned(16)));
extern Quad D_overlay_AREA00_0082B740;
extern unsigned char D_00810803;
extern float D_0081028C;
extern char D_00810350[];
extern char D_overlay_AREA00_0082ABC0[];
extern char D_overlay_AREA00_00827F50[];
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern unsigned char *func_001B6660(void *p);
extern void func_001FB0B0(int a0);
extern void func_001C6380(unsigned char *self);
extern void func_001B1B70(unsigned char *self);

void func_overlay_AREA00_00825E40(unsigned char *self) {
    Quad area = D_overlay_AREA00_0082B740;
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (func_001B1EA0(0, D_00810350, &area, 4) == 1) {
            func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_0082ABC0);
            self[5] = 1;
        }
        break;
    case 1:
        if (D_00810803 == 2) {
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
        }
        if (func_001BA1F0(self) != 0) {
            self[5] = 0;
            D_00810803 = 3;
            D_0081028C = 1.5707964f;
            func_001B6660(D_overlay_AREA00_00827F50);
            func_001FB0B0(0x17);
        }
        break;
    }
    func_001C6380(self);
    self[1] = 1;
    func_001B1B70(self);
    (*(ActorFn *)(self + 0x4C))(self);
}
