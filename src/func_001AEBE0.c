// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// HUD/display-list fade driver. Dispatches on the state short D_0028A8D0:
// cases 0/1 idle; case 2 ramps the level D_0028A8D4 down by D_0028A8D2 and
// clamps at 0 (resetting state to 0); case 3 ramps it up by D_0028A8D2 and
// clamps at 0xFF (setting state to 1). The current level is then written into
// the +0x30/+0x34/+0x38 fields of the two 0x30-byte sub-blocks of the display
// list block at D_0028A7B0 + D_00810E80[0]*0x90. Finally, unless the
// scratchpad gate at 0x70003B90 == 2 with D_008106C4 set says to skip, and
// only when the ORIGINAL state was non-zero, a GIF/DMA tag is appended through
// the display-list cursor held at D_00275670+0x14: byte +3 = 0x30,
// word +4 = block pointer, half +0 = 9, then the cursor advances by 0x10.
extern char *D_00275670;
extern char D_0028A7B0[];
extern short D_0028A8D0[16];
extern short D_0028A8D2[16];
extern int D_0028A8D4[16];
extern unsigned char D_008106C4[16];
extern short D_00810E80[16];

void func_001AEBE0(void) {
    int st;
    int i;
    char *p;

    st = D_0028A8D0[0];
    switch (st) {
    case 0:
    case 1:
        break;
    case 2:
        D_0028A8D4[0] -= D_0028A8D2[0];
        if (D_0028A8D4[0] < 0) {
            D_0028A8D4[0] = 0;
            D_0028A8D0[0] = 0;
        }
        break;
    case 3:
        D_0028A8D4[0] += D_0028A8D2[0];
        if (D_0028A8D4[0] >= 0xFF) {
            D_0028A8D4[0] = 0xFF;
            D_0028A8D0[0] = 1;
        }
        break;
    }
    p = D_0028A7B0 + (D_00810E80[0] * 0x90);
    i = 0;
    do {
        i += 1;
        *(int *)(p + 0x30) = D_0028A8D4[0];
        *(int *)(p + 0x34) = D_0028A8D4[0];
        *(int *)(p + 0x38) = D_0028A8D4[0];
        p += 0x30;
    } while (i < 2);
    if (((*(unsigned char *)0x70003B90 != 2) || (D_008106C4[0] == 0)) && (st != 0)) {
        char *blk = D_0028A7B0 + (D_00810E80[0] * 0x90);
        int *dlp = (int *)(D_00275670 + 0x14);
        *(char *)(*dlp + 3) = 0x30;
        *(int *)(*dlp + 4) = (int)blk;
        *(short *)(*dlp + 0) = 9;
        *dlp = *dlp + 0x10;
    }
}
