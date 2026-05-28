// Overlay-entry pad: single nop at vram 0x00823500. The remaining 15 nops
// at 0x00823504..0x0082353C are emitted as data by splat (a 64-byte nop
// sled / reserved vector-table area in every overlay; see docs/OVERLAYS.md).
asm void func_overlay_AREA07_00823500(void) {
    nop
}
