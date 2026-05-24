// CFLAGS: -O4,p -sdatathreshold 0
// Increment a byte at absolute address 0x008107F4.
extern unsigned char D_008107F4;

void func_overlay_AREA13_00824120(void) {
    D_008107F4 = D_008107F4 + 1;
}
