// CFLAGS: -O4,p -sdatathreshold 4
// Sets up a per-slot DMA/command descriptor: chooses a payload base address
// from the mode, then fills the descriptor that lives at +0x10 of the slot's
// entry (qwc=8, type byte 0x30, payload pointer) and bumps its tail by 0x10.
extern int D_00275674;
extern char *D_00275670;

void func_00207D00(int slot, int mode) {
    int payload;
    char *entry;

    if (mode == 3) goto M3;
    if (mode == 2) goto M2;
    if (mode == 1) goto M1;
    payload = D_00275674 + 0x720;
    goto setup;
M1:
    payload = D_00275674 + 0x7A0;
    goto setup;
M2:
    payload = D_00275674 + 0x820;
    goto setup;
M3:
    payload = D_00275674 + 0x6A0;
setup:
    entry = D_00275670 + (slot << 2);
    *(char *)(*(char **)(entry + 0x10) + 0x3) = 0x30;
    *(int *)(*(char **)(entry + 0x10) + 0x4) = payload;
    *(short *)(*(char **)(entry + 0x10) + 0x0) = 8;
    *(char **)(entry + 0x10) = *(char **)(entry + 0x10) + 0x10;
}
