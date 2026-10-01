// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031C0 0x700031C4 0x700031C8 0x700031D0
// Applies the frame's carry velocity (scratchpad 0x700031C0..C8) to the
// actor's position +0xB0/+0xB4/+0xB8. Skipped when the current stage record
// (0x700031D0) has a +0x1A high byte of 0x40 or 0x80, and skipped for the
// one actor whose class byte +4 is 1, kind +5 is 0x1E and +0x1F1 is 1.
extern char *D_700031D0;
extern float D_700031C0;
extern float D_700031C4;
extern float D_700031C8;

void func_00176BE0(unsigned char *actor) {
    int mode = *(short *)(D_700031D0 + 0x1A) & 0xFF00;

    if (mode != 0x4000 && mode != 0x8000
        && (actor[4] != 1 || actor[5] != 0x1E || actor[0x1F1] != 1)) {
        *(float *)(actor + 0xB0) += D_700031C0;
        *(float *)(actor + 0xB4) += D_700031C4;
        *(float *)(actor + 0xB8) += D_700031C8;
    }
}
