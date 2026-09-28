// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x008263C0 (splat/link name 00826380; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 0 spawns func_001B6660(0x827A30) offset by +0xB0 and sets
//  +0x30 = 0x82CCE0; state 1 starts script D_00246F20 (event 0x8000000A in
//  D_00246FB4) on +0xB bit 2, keeps 0x82CCE0 at +0xB0 + (-16.1, 12, -40.4),
//  follows the record at +0x1C (y + 1) and emits two points
//  (func_001F5940(9, ...)) at +0xB0 + (-11.017, 26.99, -41 / -36.954).
typedef void (*ActorFn)(unsigned char *);
extern int D_00246FB4;
extern char D_00246F20[];
extern float D_700038A0[4];
extern char D_overlay_AREA00_00827A30[];
extern float D_overlay_AREA00_0082CCE0[3];
extern int func_001B0FD0(unsigned char *self);
extern unsigned char *func_001B6660(void *p);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_001C6380(unsigned char *self);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *a1);
extern void func_001B1B70(unsigned char *self);
extern void func_001F5940(int a0, float *a1, int a2);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00826380(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    unsigned char *e;
    unsigned char *o;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            if ((e = func_001B6660(D_overlay_AREA00_00827A30)) != 0) {
                func_001028D0(e + 0xA0, e + 0xB0, self + 0xB0);
                *(int *)(e + 0x20) = *(int *)(self + 0x14);
            }
            self[0] = 1;
            self[8] = 1;
            *(float **)(self + 0x30) = D_overlay_AREA00_0082CCE0;
            func_001C6380(self);
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                D_00246FB4 = 0x8000000A;
                func_001BA1A0(talk, (unsigned char *)D_00246F20);
                self[5]++;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                self[5] = 0;
                self[0xB] = 0;
            }
            break;
        }
        D_overlay_AREA00_0082CCE0[0] = -16.099998f + *(float *)(self + 0xB0);
        D_overlay_AREA00_0082CCE0[1] = 12.0f + *(float *)(self + 0xB4);
        D_overlay_AREA00_0082CCE0[2] = -40.400024f + *(float *)(self + 0xB8);
        o = *(unsigned char **)(self + 0x1C);
        if (*(float *)(self + 0xB0) != *(float *)(o + 0xB0) ||
            *(float *)(self + 0xB4) != 1.0f + *(float *)(o + 0xB4)) {
            *(float *)(self + 0xB0) = *(float *)(o + 0xB0);
            *(float *)(self + 0xB4) = 1.0f + *(float *)(o + 0xB4);
            func_001C6380(self);
            func_001A2370(self, self + 0xD0);
        }
        func_001B1B70(self);
        self[1] = 1;
        (*(ActorFn *)(self + 0x4C))(self);
        D_700038A0[3] = 1.0f;
        D_700038A0[0] = -11.017f + *(float *)(self + 0xB0);
        D_700038A0[1] = 26.99f + *(float *)(self + 0xB4);
        D_700038A0[2] = -41.0f + *(float *)(self + 0xB8);
        func_001F5940(9, D_700038A0, 0);
        D_700038A0[2] = -36.954f + *(float *)(self + 0xB8);
        func_001F5940(9, D_700038A0, 0);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
