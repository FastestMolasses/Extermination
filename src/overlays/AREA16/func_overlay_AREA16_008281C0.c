// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00828200 (splat/link name 008281C0; overlay code is
// linked 0x40 below where it runs), 0x204 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: (runtime 0x828200, the child behaviour) state 0: state 3 when
//  area/sub-area (D_00810700, D_00810701) is 0x10/0x00; else after
//  func_001B0FD0 func_001CA5E0(self, +0x44, 1) and func_001C6380. State 1:
//  func_001C6380, saves the position/matrix to scratchpad, then by +0xD (6,
//  0x25, 0x14) draws 4 / 4 / 16 parts: each translates the matrix by a table
//  point (0x82C2B0 / 0x82C300 / 0x82C380), sets the model from the id table
//  (0x82C2A0 / 0x82C2F0 / 0x82C340) with func_001C6120(D_0028A59C, id), resets
//  the +0x110 matrix, func_001C6380, func_001B17A0, +0x4C, and restores the
//  saved position. State 3 func_001AFC10.
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int D_0028A59C;
extern char D_700038A0[];
extern char D_700036A0[];
extern int D_overlay_AREA16_0082C2A0[];
extern float D_overlay_AREA16_0082C2B0[][4];
extern int D_overlay_AREA16_0082C2F0[];
extern float D_overlay_AREA16_0082C300[][4];
extern int D_overlay_AREA16_0082C340[];
extern float D_overlay_AREA16_0082C380[][4];
extern int func_001B0FD0(unsigned char *self);
extern void func_001CA5E0(unsigned char *self, int a1, int a2);
extern void func_001C6380(unsigned char *self);
extern void func_00102948(void *dst, void *src);
extern void func_00102958(void *dst, void *src);
extern void func_001026A0(void *dst, void *m, void *v);
extern int func_001C6120(int a0, int a1);
extern void func_001029C0(void *m);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_008281C0(unsigned char *self) {
    int i;
    int n;
    int *ids;
    float (*pts)[4];
    switch (self[4]) {
    case 0:
        if ((D_00810700 << 8) + D_00810701 == 0x1000) {
            self[4] = 3;
            break;
        }
        if (func_001B0FD0(self) == 0) {
            func_001CA5E0(self, *(int *)(self + 0x44), 1);
            func_001C6380(self);
        }
        break;
    case 1:
        func_001C6380(self);
        func_00102948(D_700038A0, self + 0xB0);
        func_00102958(D_700036A0, self + 0xD0);
        switch (self[0xD]) {
        case 6:
            ids = D_overlay_AREA16_0082C2A0;
            n = 4;
            pts = D_overlay_AREA16_0082C2B0;
            break;
        case 0x25:
            ids = D_overlay_AREA16_0082C2F0;
            n = 4;
            pts = D_overlay_AREA16_0082C300;
            break;
        case 0x14:
            ids = D_overlay_AREA16_0082C340;
            n = 16;
            pts = D_overlay_AREA16_0082C380;
            break;
        }
        for (i = 0; i < n; i++) {
            func_001026A0(self + 0xB0, self + 0xD0, *pts);
            func_001CA5E0(self, func_001C6120(D_0028A59C, *ids), 1);
            func_001029C0(*(void **)(self + 0x110));
            func_001C6380(self);
            func_001B17A0(self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
            func_00102948(self + 0xB0, D_700038A0);
            func_00102958(self + 0xD0, D_700036A0);
            ids++;
            pts++;
        }
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
