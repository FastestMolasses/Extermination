// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA08 overlay, runtime 0x008249F0 (splat/link name 008249B0; overlay code
//  is linked 0x40 below where it runs), 0x1C4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Role: sub 0 / 1 placements [27], [28], [29]; the AREA07 0x823DA0 shape with
//  +0xB0 against 300: at spawn +8 = 3, +0x30 = &D_00275990 (+0xB0 <= 300) or
//  &D_00275998 with +0x9E = 3; state 1 waits for func_001B2140 only when
//  +0xB0 > 300; script 0x827170 when +0xB0 < 300, else 0x827330.
typedef void (*ActorFn)(unsigned char *);
extern int D_00275990;
extern int D_00275998;
extern char D_overlay_AREA08_00827170[];
extern char D_overlay_AREA08_00827330[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001B2140(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA08_008249B0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[8] = 3;
        self[0] = 1;
        self[4] = 1;
        if (!(*(float *)(self + 0xB0) <= 300.0f)) {
            *(int **)(self + 0x30) = &D_00275998;
            self[0x9E] = 3;
        } else {
            *(int **)(self + 0x30) = &D_00275990;
        }
        break;
    case 1:
        if (!(*(float *)(self + 0xB0) <= 300.0f) && func_001B2140(self) == 0) {
            break;
        }
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                if (*(float *)(self + 0xB0) < 300.0f) {
                    func_001BA1A0(talk, D_overlay_AREA08_00827170);
                } else {
                    func_001BA1A0(talk, D_overlay_AREA08_00827330);
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
