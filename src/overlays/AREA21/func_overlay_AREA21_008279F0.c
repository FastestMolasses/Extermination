// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00827A30 (splat/link name 008279F0; overlay code
//  is linked 0x40 below where it runs), 0x294 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 placement [57]. As 0x827880 with a 65-frame count; +5 2
//  places three 0x8000002F effects at (-25 / 0 / 25, 12, 0) from +0xB0, then
//  effect 0x8000005E and func_001F02C0(+0xB0, 0x44B, 800.0).
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern unsigned char D_0081080E[];
extern float D_700038A0[4];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001EFD90(int id, void *pos, void *dir);
extern void func_001EFD20(int id, void *pos);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001F02C0(void *pos, int id, float f12);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_008279F0(unsigned char *self) {
    int i;
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
            if (S16(0x28) >= 0x41) {
                self[5]++;
            }
            func_001B1B70(self);
            (*(ActorFn *)(self + 0x4C))(self);
            break;
        case 2:
            for (i = 0; i < 3; i++) {
                switch (i) {
                case 0:
                    D_700038A0[0] = -25.0f;
                    D_700038A0[1] = 12.0f;
                    D_700038A0[2] = 0.0f;
                    D_700038A0[3] = 1.0f;
                    break;
                case 1:
                    D_700038A0[0] = 0.0f;
                    D_700038A0[1] = 12.0f;
                    D_700038A0[2] = 0.0f;
                    D_700038A0[3] = 1.0f;
                    break;
                case 2:
                    D_700038A0[0] = 25.0f;
                    D_700038A0[1] = 12.0f;
                    D_700038A0[2] = 0.0f;
                    D_700038A0[3] = 1.0f;
                    break;
                }
                func_001028B8(D_700038A0, D_700038A0, self + 0xB0);
                func_001EFD20(0x8000002F, D_700038A0);
            }
            D_700038A0[0] = 0.0f;
            D_700038A0[1] = 3.1415927f;
            D_700038A0[2] = 0.0f;
            D_700038A0[3] = 1.0f;
            func_001EFD90(0x8000005E, self + 0xB0, D_700038A0);
            self[4] = 3;
            func_001F02C0(self + 0xB0, 0x44B, 800.0f);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
