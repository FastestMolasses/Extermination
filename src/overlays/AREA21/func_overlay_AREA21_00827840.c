// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00827880 (splat/link name 00827840; overlay code
//  is linked 0x40 below where it runs), 0x1A8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 placement [56]. +5 0 waits for D_0081080E == 0x10, +5 1
//  counts +0x28 to 255, +5 2: effect 0x8000004C at +0xB0 with (0, pi, 0, 1),
//  effect 0x80000041 at +0xB0, state 3, func_001F02C0(+0xB0, 0x44D, 600.0).
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern unsigned char D_0081080E[];
extern float D_700038A0[4];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001EFD90(int id, void *pos, void *dir);
extern void func_001EFD20(int id, void *pos);
extern void func_001F02C0(void *pos, int id, float f12);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00827840(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[5] = 0;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_0081080E[0] == 0x10) {
                S16(0x28) = 0;
                self[5]++;
            }
            func_001B1B70(self);
            (*(ActorFn *)(self + 0x4C))(self);
            break;
        case 1:
            S16(0x28)++;
            if (S16(0x28) >= 0xFF) {
                self[5]++;
            }
            func_001B1B70(self);
            (*(ActorFn *)(self + 0x4C))(self);
            break;
        case 2:
            D_700038A0[0] = 0.0f;
            D_700038A0[1] = 3.1415927f;
            D_700038A0[2] = 0.0f;
            D_700038A0[3] = 1.0f;
            func_001EFD90(0x8000004C, self + 0xB0, D_700038A0);
            func_001EFD20(0x80000041, self + 0xB0);
            self[4] = 3;
            func_001F02C0(self + 0xB0, 0x44D, 600.0f);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
