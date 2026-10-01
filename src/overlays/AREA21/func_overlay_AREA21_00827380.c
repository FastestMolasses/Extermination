// NEARMISS func_overlay_AREA21_00827380 (96.50%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA21 overlay, runtime 0x008273C0 (splat/link name 00827380; overlay code
//  is linked 0x40 below where it runs), 0x280 bytes.
// Role: sub 0 placements [49], [50]. As 0x827640 (offsets 0.3 / 0.2 /
//  0.4) plus func_001FC3C0(self, +0x1F8, 0x450, 150.0, 4096.0) and the
//  *(D_00275B40 + 4) object's +0x78 turning by -0.523 a frame.
// Divergence: the original builds the func_001FC3C0 block pointer
// (self + 0x1F0) + 8 last, in the call's slot, and materialises 150.0
// first (it also fills the jump-table default slot); mwcc 2.3.3 computes
// the pointer first and puts it in that slot. Pointer locals, casts and
// int-staged floats were tried. The jump table at 0x82DC40 stays in the
// data assembly.
typedef void (*ActorFn)(unsigned char *);
#define F(o) (*(float *)(self + (o)))
#define REC(o) (*(unsigned char **)(D_00275B40 + (o)))
extern char *D_00275B40;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001FC3C0(unsigned char *self, void *blk, int id, float f12, float f13);
extern float func_001B1470(float a);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00827380(unsigned char *self) {
    int *blk = (int *)(self + 0x1F0);
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        func_001C6380(self);
        F(0x1F0) = F(0xB0);
        F(0x1F4) = F(0xB4);
        *(int *)(self + 0x1F8) = -1;
        break;
    case 1:
        *(short *)(self + 0x28) += 1;
        switch (*(short *)(self + 0x28) & 7) {
        case 0:
            F(0xB0) = F(0x1F0);
            F(0xB4) = F(0x1F4);
            break;
        case 1:
            F(0xB0) = F(0x1F0) - 0.3f;
            F(0xB4) = F(0x1F4);
            break;
        case 2:
            F(0xB0) = F(0x1F0);
            F(0xB4) = F(0x1F4) - 0.2f;
            break;
        case 3:
            F(0xB0) = F(0x1F0);
            F(0xB4) = 0.2f + F(0x1F4);
            break;
        case 4:
            F(0xB0) = 0.3f + F(0x1F0);
            F(0xB4) = F(0x1F4);
            break;
        case 5:
            F(0xB0) = F(0x1F0) - 0.2f;
            F(0xB4) = 0.4f + F(0x1F4);
            break;
        case 6:
            F(0xB0) = 0.3f + F(0x1F0);
            F(0xB4) = F(0x1F4) - 0.2f;
            break;
        case 7:
            F(0xB0) = 0.2f + F(0x1F0);
            F(0xB4) = 0.2f + F(0x1F4);
            break;
        }
        func_001FC3C0(self, blk + 2, 0x450, 150.0f, 4096.0f);
        *(float *)(REC(4) + 0x78) = func_001B1470(*(float *)(REC(4) + 0x78) - 0.523f);
        func_001C6380(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
