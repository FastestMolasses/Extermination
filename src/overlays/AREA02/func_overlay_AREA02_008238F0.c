// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00823930 (splat/link name 008238F0; overlay
// code is linked 0x40 below where it runs), 0x50 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Role: dispatch on +2 & 0x1F: 9 calls 0x823980, 4 calls 0x824020. At the
//  calls a1 holds the masked byte and a2 the raw byte (written as extra
//  arguments; the callees read only a0).
extern void func_overlay_AREA02_00823980(unsigned char *self, int kind, int raw);
extern void func_overlay_AREA02_00824020(unsigned char *self, int kind, int raw);

void func_overlay_AREA02_008238F0(unsigned char *self) {
    int raw = self[2];
    int kind = raw & ~0xE0;
    if (kind == 9) {
        func_overlay_AREA02_00823980(self, kind, raw);
    } else if (kind == 4) {
        func_overlay_AREA02_00824020(self, kind, raw);
    }
}
