// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003B92
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.78% NEARMISS).
// Fixes read from the original instructions:
// - The callees take the object: func_001B0FD0(self) and func_001B1B70(self)
//   (definitions in src/), and the switch reads self[4] directly as in the
//   byte-matched twin func_001C4AF0; the old no-argument externs and the
//   unsigned-char state local misplaced the self copy and the case chain.
// - The scratchpad flag byte at 0x70003B92 is the relocated extern
//   D_70003B92 (idiom-32); a literal address let mwcc speculate its upper
//   half into the case-1 compare's delay slot.
//
// State handler: state 0 runs func_001C6380(self) unless func_001B0FD0(self)
// returns nonzero; state 1 runs func_001B1B70(self) and the callback at
// self+0x4C when the scratchpad flag 0x70003B92 is clear, or when it is set
// and the table byte D_008107D8[self[3]] differs from the short at self+0x56;
// states 2 and 3 run func_001AFC10(self).
extern int func_001B0FD0(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001AFC10(void *self);
extern unsigned char D_008107D8[];
extern unsigned char D_70003B92;

void func_001C4A00(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
        }
        break;
    case 1:
        if (D_70003B92 == 0) {
            func_001B1B70(self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        } else if (D_008107D8[self[3]] != *(short *)(self + 0x56)) {
            func_001B1B70(self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
