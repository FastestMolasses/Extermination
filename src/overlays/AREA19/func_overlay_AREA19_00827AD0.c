// CFLAGS: -O4,p -sdatathreshold 0
// Set byte at absolute address 0x0081081E to 1, return 1.
extern unsigned char D_0081081E;

int func_overlay_AREA19_00827AD0(void) {
    D_0081081E = 1;
    return 1;
}
