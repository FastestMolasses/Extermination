// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Synthesises the left stick from the d-pad (pad block translation, em_input
// 001B5940 family). pad points at the button halfword, out at the translated
// pad record. out+0x17 is set to 3 (stick active), then the d-pad direction
// bits pick the stick bytes out+0x24 (X) / out+0x25 (Y): 0x1000 up, 0x4000
// down, 0x2000 right, 0x8000 left; diagonals use 0x10 / 0xF0. With no
// direction held the stick is centred (0x80, 0x80) and out+0x17 returns to 0.
void func_001B5E20(unsigned short *pad, unsigned char *out) {
    unsigned short b;

    out[0x17] = 3;
    b = *pad;
    if (b & 0x1000) {
        if (b & 0x2000) {
            out[0x24] = 0xF0;
            out[0x25] = 0x10;
            return;
        }
        if (b & 0x8000) {
            out[0x24] = 0x10;
            out[0x25] = 0x10;
            return;
        }
        out[0x24] = 0x80;
        out[0x25] = 0;
        return;
    }
    if (b & 0x4000) {
        if (b & 0x2000) {
            out[0x24] = 0xF0;
            out[0x25] = 0xF0;
            return;
        }
        if (b & 0x8000) {
            out[0x24] = 0x10;
            out[0x25] = 0xF0;
            return;
        }
        out[0x24] = 0x80;
        out[0x25] = 0xFF;
        return;
    }
    if (b & 0x2000) {
        out[0x24] = 0xFF;
        out[0x25] = 0x80;
        return;
    }
    if (b & 0x8000) {
        out[0x24] = 0;
        out[0x25] = 0x80;
        return;
    }
    out[0x24] = 0x80;
    out[0x25] = 0x80;
    out[0x17] = 0;
}
