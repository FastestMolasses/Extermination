// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// GS packet helper: appends a TRXREG register write (A+D register 0x52) with
// func_00205A50, value w | h << 32 (each zero-extended). Returns the
// advanced packet pointer.
extern char *func_00205A50(char *pkt, int reg, unsigned long long value);

char *func_00205F20(char *pkt, unsigned int w, unsigned int h) {
    return func_00205A50(pkt, 0x52, (unsigned long long)h << 32 | (unsigned long long)w);
}
