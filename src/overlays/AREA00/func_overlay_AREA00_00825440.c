// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00825480 (splat/link name 00825440; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 0 (after func_001B0FD0 returns 0) sets +0 = 1, +8 = 1,
//  +0x30 = 0x82A520 and calls func_001C5570(self, {0, 1, 0, 1}, 9, 0); state 1
//  starts script 0x8299E0 on +0xB bit 2 and, when it ends, sets bit 0 of
//  D_008107DC and clears +0xB and +5.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_008107DC;
extern float D_700038A0[4];
extern char D_overlay_AREA00_0082A520[];
extern char D_overlay_AREA00_008299E0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001C5570(unsigned char *self, float *v, int a2, int a3);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00825440(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            self[0] = 1;
            self[8] = 1;
            *(char **)(self + 0x30) = D_overlay_AREA00_0082A520;
            func_001C6380(self);
            D_700038A0[0] = 0.0f;
            D_700038A0[1] = 1.0f;
            D_700038A0[2] = 0.0f;
            D_700038A0[3] = 1.0f;
            func_001C5570(self, D_700038A0, 9, 0);
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                self[5]++;
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_008299E0);
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                D_008107DC |= 1;
                self[0xB] = 0;
                self[5] = 0;
            }
            break;
        }
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
