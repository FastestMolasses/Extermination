// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00825240 (splat/link name 00825200; overlay code
//  is linked 0x40 below where it runs), 0x1D4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Covers the splat pieces 00825200, 00825240 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-state of 0x8250F0 (D_008107F5 bit 0 clear). +5 0: with the player
//  in the area 0x82BD00: script 0x82B6E0, +5 = 1, +0x28 = 0, D_008106C0 = 0
//  when not below y 210 (nothing else that frame); outside the area the idle
//  update (func_001C64F0, func_001B17A0, func_001C68C0, the +0x4C method). +5
//  1: at count 0x424 func_001EFE00(0x8000002B, D_008106C0) when it is set;
//  when the script ends func_001AEE10(4, 0), +0x2E = 0xFFFF, D_008107F5 |= 1
//  and |= 2, func_001C67E0(self, 0, 0.0, 0.0), +5 = 0, func_001E8B40(0),
//  D_008106C0 = func_001B6660(0x82A590), func_001FAE70(0).
typedef struct { float v[16]; } Poly __attribute__((aligned(16)));
typedef void (*ActorFn)(unsigned char *);
extern Poly D_overlay_AREA19_0082BD00;
extern char D_overlay_AREA19_0082B6E0[];
extern char D_overlay_AREA19_0082A590[];
extern float D_00810350[];
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C64F0(unsigned char *self, float step);
extern void func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001EFE00(unsigned int msg, void *other);
extern void func_001AEE10(int a, int b);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001E8B40(int a);
extern void *func_001B6660(void *p);
extern void func_001FAE70(int a);

void func_overlay_AREA19_00825200(unsigned char *self) {
    Poly area = D_overlay_AREA19_0082BD00;
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (func_001B1EA0(0, D_00810350, &area, 4) == 1) {
            if (!(*(float *)0x810354 < 210.0f)) {
                func_001BA1A0(talk, D_overlay_AREA19_0082B6E0);
                self[5] = 1;
                *(short *)(self + 0x28) = 0;
                *(void **)0x8106C0 = 0;
            }
            break;
        }
        func_001C64F0(self, 1.0f);
        func_001B17A0(self);
        func_001C68C0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 1:
        if (*(short *)(self + 0x28) == 0x424 && *(void **)0x8106C0 != 0) {
            func_001EFE00(0x8000002B, *(void **)0x8106C0);
        }
        *(short *)(self + 0x28) += 1;
        if (func_001BA1F0(self) != 0) {
            func_001AEE10(4, 0);
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            *(unsigned char *)0x8107F5 |= 1;
            *(unsigned char *)0x8107F5 |= 2;
            {
                int zi = 0;
                float z = (float)zi;
                func_001C67E0(self, 0, 0.0f, z);
            }
            self[5] = 0;
            func_001E8B40(0);
            *(void **)0x8106C0 = func_001B6660(D_overlay_AREA19_0082A590);
            func_001FAE70(0);
        }
        break;
    }
}
