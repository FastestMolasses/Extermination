// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00823BC0 (splat/link name 00823B80; overlay code
//  is linked 0x40 below where it runs), 0x4C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: sub 0 placements [5] / [6]: calls 0x823C10 for +0xD 0 and 0x823D50
//  for +0xD 1 (the id is passed as a second argument, which the callees do
//  not read).
extern void func_overlay_AREA13_00823C10(unsigned char *self, int id);
extern void func_overlay_AREA13_00823D50(unsigned char *self, int id);

void func_overlay_AREA13_00823B80(unsigned char *self) {
    int id = self[0xD];
    switch (id) {
    case 0:
        func_overlay_AREA13_00823C10(self, id);
        break;
    case 1:
        func_overlay_AREA13_00823D50(self, id);
        break;
    }
}
