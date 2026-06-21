// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-frame CATEGORY-LIST PUBLISH: if class-flag 0x80 (interactive) is set
// on byte +0x02, push onto the interactive list via func_001B1DE0; then
// dispatch on class (low 5 bits of byte +0x02): 1 -> func_001B1C60,
// 2/0xA -> func_001B1CA0, 4 -> func_001B1D20, 7 -> func_001B1D60,
// 0xD -> func_001B1DA0.
//
// A C `switch` makes mwcc reproduce CW's beql-compare-chain switch
// lowering. Source case order 1, [0xA,2], 4, 7, 0xD reproduces the whole
// structure. The 991202 build stalls at 91.53% on two walls: the dead
// paddub block-head dups it coalesces, and the case-0xA beq delay slot it
// fills (delay-slot-fill wall #13). mwcc 2.3.3 leaves both as CW did and
// matches byte-for-byte from this plain-C switch.
extern void func_001B1C60(unsigned char *self);
extern void func_001B1CA0(unsigned char *self);
extern void func_001B1D20(unsigned char *self);
extern void func_001B1D60(unsigned char *self);
extern void func_001B1DA0(unsigned char *self);
extern void func_001B1DE0(unsigned char *self);

void func_001B1B70(unsigned char *self) {
    int c;
    if (self[0x2] & 0x80) func_001B1DE0(self);
    c = self[0x2] & ~0xE0;
    switch (c) {
    case 1:            func_001B1C60(self); break;
    case 0xA: case 2:  func_001B1CA0(self); break;
    case 4:            func_001B1D20(self); break;
    case 7:            func_001B1D60(self); break;
    case 0xD:          func_001B1DA0(self); break;
    }
}
