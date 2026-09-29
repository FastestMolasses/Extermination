// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00823F60 (splat/link name 00823F20; overlay code is
// linked 0x40 below where it runs), 0x110 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Role: (self, a1, a2) step callback on the object held in D_008106C0. It
//  returns 1 when that pointer is null or the object's state is 3 (the pointer
//  is then cleared). For a1[4] == 0 it copies a2 + 0x20 into D_008105D0 as
//  well as D_008101F0, copies the object's +0xB0 position into D_008105E0 as
//  well as D_00810200, puts the object in state 2 (+0x36 = 100, +5 = 0) with
//  a1[4] = 1; for a1[4] == 1 it copies the object position into D_008105E0 as
//  well as D_00810200. It returns 0 in both cases.
extern unsigned char *D_008106C0;
extern char D_008105D0[];
extern char D_008105E0[];
extern char D_008101F0[];
extern char D_00810200[];
extern void func_00102948(void *dst, void *src);

int func_overlay_AREA15_00823F20(unsigned char *self, unsigned char *a1, unsigned char *a2) {
    unsigned char *obj = D_008106C0;
    if (obj != 0) {
        if (obj[4] == 3) {
            D_008106C0 = 0;
            return 1;
        }
        switch (a1[4]) {
        case 0:
            func_00102948(D_008105D0, a2 + 0x20);
            func_00102948(D_008105E0, obj + 0xB0);
            func_00102948(D_008101F0, a2 + 0x20);
            func_00102948(D_00810200, obj + 0xB0);
            *(short *)(obj + 0x36) = 100;
            obj[4] = 2;
            obj[5] = 0;
            a1[4] = 1;
            break;
        case 1:
            func_00102948(D_008105E0, obj + 0xB0);
            func_00102948(D_00810200, obj + 0xB0);
            break;
        }
        return 0;
    }
    return 1;
}
