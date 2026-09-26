// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825FC0 (splat/link name 00825F80; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x825950 (D_008107DF 0xFF path of model 0x4B); plain
//  animation tick.
// Covers the splat pieces 00825F80, 00825FC0 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
typedef void (*ActorFn)(char *);
extern short func_001C64F0(char *self, float dt);
extern void func_001C68C0(char *self);
extern unsigned char func_001B17A0(char *self);

void func_overlay_AREA01_00825F80(char *self) {
    func_001C64F0(self, 1.0f);
    func_001C68C0(self);
    func_001B17A0(self);
    (*(ActorFn *)(self + 0x4C))(self);
}
