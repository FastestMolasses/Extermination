// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00826440 (splat/link name 00826400; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x8261A0 (unverified label: 'bridge half 3'): script 0x82B0D0, D_008104A4
//  ramp, player snap.
// Covers the splat pieces 00826400, 00826440 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081075E;
extern unsigned char D_00810760;
extern unsigned char D_00810784;
extern unsigned char D_008107E0;
extern int D_00810374;
extern float D_008104A4;
extern float D_00810350[8];
extern char D_008102B0[];
extern char D_overlay_AREA01_0082B0D0[];
extern char D_overlay_AREA01_0082CC20[];
extern char D_overlay_AREA01_0082CC60[];
extern int func_001B0FD0(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern int func_00182BF0(char *actor);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001A2370(unsigned char *actor, void *mtx);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA01_00826400(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    int *timer = (int *)(self + 0x240);
    switch (self[4]) {
    case 0:
        if (D_0081075E != 0) {
            *(float *)(self + 0xC0) = 0.0f;
        } else {
            *(float *)(self + 0xC0) = -1.04719758f;
        }
        if (D_00810760 != 0) {
            *(float *)(self + 0xC0) = 0.0f;
        }
        if (D_00810784 != 0) {
            *(float *)(self + 0xC0) = 0.0f;
        }
        func_001B0FD0(self);
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (func_001BA1C0(self, 8) == 0 && *(unsigned char *)0x70003B8D == 0 &&
                func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA01_0082CC60, 4) == 1 &&
                D_00810350[1] <= 2.0f && func_00182BF0(D_008102B0) == 0) {
                func_001BA1A0(talk, D_overlay_AREA01_0082B0D0);
                D_008107E0 = 1;
                self[5] = 1;
                D_00810374 = 0;
            }
            break;
        case 1:
            if (D_008107E0 == 0xE0) {
                switch (self[6]) {
                case 0:
                    *timer = 0;
                    self[6] = 1;
                    break;
                case 1:
                    *timer += 1;
                    if (*timer == 0x3C) {
                        D_008104A4 = 0.3f;
                    }
                    if (*timer == 0xC8) {
                        D_008104A4 = 0.6f;
                    }
                    break;
                }
            }
            if (D_008107E0 == 2) {
                switch (self[7]) {
                case 0:
                    D_00810350[0] = D_00810350[4];
                    D_00810350[1] = 0.0f;
                    D_00810350[2] = D_00810350[6];
                    self[7] = 1;
                    break;
                case 1:
                    break;
                }
            }
            *(short *)(self + 0x2A) += 1;
            if (func_001BA1F0(self) != 0) {
                D_008107E0 = 0xFF;
                D_008104A4 = 1.0f;
                self[5] = 2;
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
