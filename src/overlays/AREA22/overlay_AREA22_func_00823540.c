// CFLAGS: -O4,p -sdatathreshold 4
// AREA22 overlay init, link 0x00823540, runtime 0x00823580, 0x20 bytes: the
// only code in AREA22.BIN besides the 0x40-byte entry pad (everything after
// it is the overlay's data). Boot func_001E7780 calls runtime 0x823580 for
// area key 0x1600 (its symbol there is area_dispatch_off0080_state0300,
// shared with key 0x300/0x301 in other overlays) after it has zeroed
// D_00275C18..D_00275C2C; the two arguments are ignored. It stores four
// gp-relative boot words: D_00275C28 = 0x20, D_00275C1C = 0x00823E00 (the
// runtime address of the first byte after the loaded file image, where the
// zeroed .bss begins; the boot code indexes it as D_00275C1C + i * 0xA060 in
// func_001E9580 / func_001E9E60), D_00275C2C = 0 and D_00275C24 = 0.
// D_00275C20 and D_00275C18 keep the zero written by func_001E7780, so
// func_001E7780's closing loop over D_00275C2C records does not run.
// Byte-identical (default mwccmips, sdatathreshold 4); see
// docs/AREA22_OVERLAY.md.
extern int D_00275C1C;
extern int D_00275C24;
extern int D_00275C28;
extern int D_00275C2C;
extern char D_overlay_AREA22_00823E00[];

void overlay_AREA22_func_00823540(void) {
    D_00275C28 = 0x20;
    D_00275C1C = (int)D_overlay_AREA22_00823E00;
    D_00275C2C = 0;
    D_00275C24 = 0;
}
