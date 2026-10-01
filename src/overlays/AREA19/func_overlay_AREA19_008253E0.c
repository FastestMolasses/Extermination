// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00825420 (splat/link name 008253E0; overlay code
//  is linked 0x40 below where it runs), 0x1AC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Covers the splat pieces 008253E0, 00825420 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-state of 0x8250F0 (D_008107F5 bit 2 clear): position from
//  0x82BD80 (the word after the area 0x82BD40), +0xC4 = -0.838. +5 0: script
//  0x82BA00 and +0x2E = 0 when the player is in the area 0x82BD40 with 190 <=
//  y <= 200; +5 1: when it ends +5 = 2, +0x2E = 0xFFFF, D_008107F5 = 0xFF,
//  func_001C67E0(self, 1, 0.0, 0.0), func_001FAE70(0). While D_70003B92 == 0
//  the idle update.
typedef struct { float v[16]; } Poly __attribute__((aligned(16)));
typedef void (*ActorFn)(unsigned char *);
extern Poly D_overlay_AREA19_0082BD40;
extern char D_overlay_AREA19_0082BA00[];
extern float D_00810350[];
extern unsigned char D_70003B92;
extern void func_00102948(void *dst, void *src);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001FAE70(int a);
extern void func_001C64F0(unsigned char *self, float step);
extern void func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);

void func_overlay_AREA19_008253E0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    Poly area = D_overlay_AREA19_0082BD40;
    float y;
    func_00102948(self + 0xB0, &D_overlay_AREA19_0082BD40 + 1);
    *(float *)(self + 0xC4) = -0.83775806f;
    switch (self[5]) {
    case 0:
        if (func_001B1EA0(0, D_00810350, &area, 4) == 1) {
            y = *(float *)0x810354;
            if (!(y < 190.0f) && y <= 200.0f) {
                func_001BA1A0(talk, D_overlay_AREA19_0082BA00);
                *(short *)(self + 0x2E) = 0;
                self[5] = 1;
            }
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            self[5] = 2;
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            *(unsigned char *)0x8107F5 = 0xFF;
            func_001C67E0(self, 1, 0.0f, 0.0f);
            func_001FAE70(0);
        }
        break;
    case 2:
        break;
    }
    if (D_70003B92 == 0) {
        func_001C64F0(self, 1.0f);
        func_001B17A0(self);
        func_001C68C0(self);
        (*(ActorFn *)(self + 0x4C))(self);
    }
}
