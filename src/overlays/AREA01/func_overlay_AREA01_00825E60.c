// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825EA0 (splat/link name 00825E60; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x825950; clears +5 and sets D_008107DF = 0x80 when the
//  talk ends.
// Covers the splat pieces 00825E60, 00825EA0 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
typedef void (*ActorFn)(char *);
extern int func_001BA1F0(char *self);
extern void func_001C68C0(char *self);
extern unsigned char D_008107DF;

void func_overlay_AREA01_00825E60(char *self) {
    if (func_001BA1F0(self) != 0) {
        self[5] = 0;
        D_008107DF = 0x80;
    }
    func_001C68C0(self);
    self[1] = 1;
    (*(ActorFn *)(self + 0x4C))(self);
}
