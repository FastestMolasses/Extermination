// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA07 overlay, runtime 0x00823DA0 (splat/link name 00823D60; overlay code
//  is linked 0x40 below where it runs), 0x1D8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA07; lane OVLC).
// Role: sub 0 / 1 placements [17], [18]. State 0: after func_001B0FD0,
//  func_001C6380, +0 = 1, state 1; +0xB8 < 600: +0x9E = 3, +0x30 = 0x827180,
//  +8 = 2; else +0x30 = &D_00275970, +8 = 3. State 1 (with +0xB8 < 600 only
//  while func_001B2140(self)): +5 0: on +0xB bit 2 script 0x8271A0 when
//  D_00810358 < 600 else 0x827360, +5 1; +5 1: at the script end +0xB = +5 =
//  0. Then func_001B17A0 and the +0x4C method. States 2 / 3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern int D_00275970;
extern float D_00810358[];
extern char D_overlay_AREA07_00827180[];
extern char D_overlay_AREA07_008271A0[];
extern char D_overlay_AREA07_00827360[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001B2140(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA07_00823D60(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[0] = 1;
        self[4] = 1;
        if (*(float *)(self + 0xB8) < 600.0f) {
            self[0x9E] = 3;
            *(char **)(self + 0x30) = D_overlay_AREA07_00827180;
            self[8] = 2;
        } else {
            *(int **)(self + 0x30) = &D_00275970;
            self[8] = 3;
        }
        break;
    case 1:
        if (*(float *)(self + 0xB8) < 600.0f && func_001B2140(self) == 0) {
            break;
        }
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                if (D_00810358[0] < 600.0f) {
                    func_001BA1A0(talk, D_overlay_AREA07_008271A0);
                } else {
                    func_001BA1A0(talk, D_overlay_AREA07_00827360);
                }
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
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
