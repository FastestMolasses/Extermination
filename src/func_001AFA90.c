// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Actor POOL ALLOC. Pops a 0x2F0-byte actor from the free list (head
// D_00275BC4, count D_00275BC8) and initializes it: status +0x00 = 2,
// class byte +0x02 (low 5 bits class, bits 5-7 flags), +0x14 canonical
// self-pointer, scale +0x60..0x6C = 1.0 x4, +0x70/74 = 0, +0x78/7C = 1.0,
// anim scale +0x80..0x8C = 1.0 x4, pos +0xB0..B8 = 0 / +0xBC = 1.0,
// +0x94 = -1 h, +0x96 = 0 h, +0x99/9A/9C = 0, +0x30/52/54/56/58 = 0,
// +0x5C = 0x00010101; links into the active list via func_001AFA50.
// Class 0xC (transient FX) is refused when fewer than 10 actors remain
// free (reserve). Class 2 additionally latches the area sub-state bytes
// D_00810701/D_00810702 into +0x9D/+0x9E. Returns the actor or 0.
// KEY STEERING (s84): load `self = D_00275BC4` AFTER the reserve-check block,
// not before -- this lets mwcc keep the saves contiguous, sink `sq s0` into
// the c==0xC bne delay slot, and emit the self-load in its natural fall-
// through position, exactly matching CW's prologue schedule. sdatathreshold 8
// makes D_00275BC4/BC8 gp-relative (CW's %gp_rel form). 991202 stalls at
// 88.22% (the documented prologue-scheduling wall); mwcc 2.3.3 byte-matches.
extern void func_001AFA50(unsigned char *actor);  // active-list link
extern unsigned char *D_00275BC4;   // free-list head
extern short D_00275BC8;             // free count
extern unsigned char D_00810701[];   // area sub-state
extern unsigned char D_00810702[];

unsigned char *func_001AFA90(int cls) {
    unsigned char *self;
    unsigned char c;

    c = (unsigned char)(cls & ~0xE0);
    if (c == 0xC) {
        if (D_00275BC8 < 10) {
            return 0;
        }
    }
    self = D_00275BC4;
    if (self != 0) {
        D_00275BC8 -= 1;
        D_00275BC4 = *(unsigned char **)(self + 0x1C);
        *(unsigned char **)(self + 0x14) = self;
        self[0x0] = 2;
        self[0x2] = cls;
        *(int *)(self + 0x60) = 0x3F800000;
        *(int *)(self + 0x64) = 0x3F800000;
        *(int *)(self + 0x68) = 0x3F800000;
        *(int *)(self + 0x6C) = 0x3F800000;
        *(int *)(self + 0x80) = 0x3F800000;
        *(int *)(self + 0x84) = 0x3F800000;
        *(int *)(self + 0x88) = 0x3F800000;
        *(int *)(self + 0x8C) = 0x3F800000;
        *(int *)(self + 0xB0) = 0;
        *(int *)(self + 0xB4) = 0;
        *(int *)(self + 0xB8) = 0;
        *(int *)(self + 0xBC) = 0x3F800000;
        *(int *)(self + 0x70) = 0;
        *(int *)(self + 0x74) = 0;
        *(int *)(self + 0x78) = 0x3F800000;
        *(int *)(self + 0x7C) = 0x3F800000;
        self[0x9A] = 0;
        self[0x99] = 0;
        *(short *)(self + 0x94) = -1;
        *(short *)(self + 0x96) = 0;
        func_001AFA50(self);
        *(int *)(self + 0x30) = 0;
        *(short *)(self + 0x54) = 0;
        *(short *)(self + 0x56) = 0;
        *(short *)(self + 0x52) = 0;
        *(int *)(self + 0x58) = 0;
        *(int *)(self + 0x5C) = 0x00010101;
        self[0x9C] = 0;
        if (c == 2) {
            self[0x9D] = D_00810701[0];
            self[0x9E] = D_00810702[0];
        }
        return self;
    }
    return 0;
}
