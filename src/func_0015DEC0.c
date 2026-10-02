// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Is the current stage record (0x700031D0) +0x1A mode word a 0x20xx mode
// other than low bytes 0x46 and 0x32? Returns 1 if so, else 0.

int func_0015DEC0(void) {
    short m = *(short *)(*(char **)0x700031D0 + 0x1A);
    int lo = m & 0xFF;

    if (lo != 0x46 && lo != 0x32 && (m & 0xFF00) == 0x2000) {
        return 1;
    }
    return 0;
}
