// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x008239D0 (splat/link name 00823990; overlay code
//  is linked 0x40 below where it runs), 0x1A4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: group 0x826890 member (x4): state 1 a func_001F4E20 point at +0x100
//  by +0x94: 5 / 6 / 7 colour (0x30, 0x80, 0x30, 0x80) size 5.0, 8 colour
//  (0x80, 0x50, 0x30, 0x80) size 8.0.
extern int D_700038B0[];
extern void func_001F4E20(void *pos, void *rgba, float f12);

void func_overlay_AREA17_00823990(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        switch (*(short *)(self + 0x94)) {
        case 5:
            D_700038B0[0] = 0x30;
            D_700038B0[1] = 0x80;
            D_700038B0[2] = 0x30;
            D_700038B0[3] = 0x80;
            func_001F4E20(self + 0x100, D_700038B0, 5.0f);
            break;
        case 6:
            D_700038B0[0] = 0x30;
            D_700038B0[1] = 0x80;
            D_700038B0[2] = 0x30;
            D_700038B0[3] = 0x80;
            func_001F4E20(self + 0x100, D_700038B0, 5.0f);
            break;
        case 7:
            D_700038B0[0] = 0x30;
            D_700038B0[1] = 0x80;
            D_700038B0[2] = 0x30;
            D_700038B0[3] = 0x80;
            func_001F4E20(self + 0x100, D_700038B0, 5.0f);
            break;
        case 8:
            D_700038B0[0] = 0x80;
            D_700038B0[1] = 0x50;
            D_700038B0[2] = 0x30;
            D_700038B0[3] = 0x80;
            func_001F4E20(self + 0x100, D_700038B0, 8.0f);
            break;
        }
        break;
    case 2:
    case 3:
        break;
    }
}
