// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// GS packet helper: appends a UV register write (A+D register 3) with
// func_00205A50, value u | v << 16 (each zero-extended). Returns the
// advanced packet pointer.
extern char *func_00205A50(char *pkt, int reg, unsigned long long value);

char *func_00205C60(char *pkt, unsigned int u, unsigned int v) {
    return func_00205A50(pkt, 3, (unsigned long long)u | (unsigned long long)v << 16);
}
