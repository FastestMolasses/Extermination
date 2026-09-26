// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00826200 (splat/link name 008261C0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x8261A0 (unverified label: 'bridge half 2'): script 0x82B4D0, sound 0x8A9,
//  quad tests 0x82CC20/0x82CC60.
// Covers the splat pieces 008261C0, 00826200 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081075E;
extern unsigned char D_00810760;
extern unsigned char D_00810784;
extern unsigned char D_008107E0;
extern char D_00810350[];
extern char D_overlay_AREA01_0082B4D0[];
extern char D_overlay_AREA01_0082CC20[];
extern char D_overlay_AREA01_0082CC60[];
extern int func_001B0FD0(unsigned char *self);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern void func_001FC3C0(void *a0, void *a1, int a2, float f12, float f13);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FC520(void *a0);
extern void func_001C6380(unsigned char *self);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001B1B70(unsigned char *self);
extern void func_001A2370(unsigned char *actor, void *mtx);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA01_008261C0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    unsigned char *snd = self + 0x240;
    switch (self[4]) {
    case 0:
        if (D_0081075E != 0) {
            *(float *)(self + 0xC0) = 0.174532935f;
        } else {
            *(float *)(self + 0xC0) = 1.04719758f;
        }
        if (D_00810760 != 0) {
            *(float *)(self + 0xC0) = 1.04719758f;
        }
        if (D_00810784 != 0) {
            *(float *)(self + 0xC0) = 0.0f;
        }
        func_001B0FD0(self);
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810760 != 0xFF && D_008107E0 != 0) {
                func_001BA1A0(talk, D_overlay_AREA01_0082B4D0);
                self[5] = 1;
                *(int *)snd = -1;
                *(short *)(self + 0x28) = 0;
            }
            break;
        case 1:
            func_001FC3C0(self, snd, 0x8A9, 3000.0f, 4096.0f);
            if (func_001BA1F0(self) != 0) {
                self[5] = 2;
                func_001FC520(snd);
            }
            break;
        case 2:
            break;
        }
        func_001C6380(self);
        if (func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA01_0082CC20, 4) == 1 ||
            func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA01_0082CC60, 4) == 1) {
            self[1] = 1;
            func_001B1B70(self);
            func_001A2370(self, self + 0xD0);
        } else {
            func_001B1B70(self);
            func_001A2370(self, self + 0xD0);
        }
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
