// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// GS packet helper: appends a XYOFFSET_1 register write (A+D register 0x18) with
// func_00205A50, value x | y << 32 (each zero-extended). Returns the
// advanced packet pointer.
extern char *func_00205A50(char *pkt, int reg, unsigned long long value);

char *func_00205E10(char *pkt, unsigned int x, unsigned int y) {
    return func_00205A50(pkt, 0x18, (unsigned long long)x | (unsigned long long)y << 32);
}
