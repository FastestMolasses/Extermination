// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825BE0 (splat/link name 00825BA0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x825950; waits for the player inside the quad at
//  0x82B090, then runs script 0x82AA90.
// Covers the splat pieces 00825BA0, 00825BE0 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
typedef void (*ActorFn)(unsigned char *);
typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 p[4]; } Quad;
extern Quad D_overlay_AREA01_0082B090;
extern char D_overlay_AREA01_0082AA90[];
extern char D_00810350[];
extern char D_008105E0[];
extern unsigned char D_008107DF;
extern void func_001C63E0(unsigned char *self, short a1);
extern int func_001B1EA0(int a, void *b, void *c, int d);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_00102948(void *dst, void *src);
extern void func_overlay_AREA01_00826010(unsigned char *self);
extern short func_001C64F0(unsigned char *self, float dt);
extern void func_001C68C0(unsigned char *self);

void func_overlay_AREA01_00825BA0(unsigned char *self) {
    Quad area = D_overlay_AREA01_0082B090;
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        func_001C63E0(self, 7);
        self[5] = 1;
        break;
    case 1:
        if (func_001B1EA0(0, D_00810350, &area, 4) != 0) {
            self[5] = 2;
            D_008107DF = 1;
            func_001BA1A0(talk, D_overlay_AREA01_0082AA90);
        }
        break;
    case 2:
        if (func_001BA1F0(self) != 0) {
            self[5] = 0;
            D_008107DF = 2;
            func_00102948(D_008105E0, *(char **)(self + 0x18) + 0xB0);
        }
        break;
    }
    func_overlay_AREA01_00826010(self);
    func_001C64F0(self, 1.0f);
    func_001C68C0(self);
    self[1] = 1;
    (*(ActorFn *)(self + 0x4C))(self);
}
