// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// GS packet helper: appends a PRMODECONT register write (A+D register 0x1A) with
// func_00205A50, value ac (zero-extended). Returns the advanced packet
// pointer.
extern char *func_00205A50(char *pkt, int reg, unsigned long long value);

char *func_00205E30(char *pkt, unsigned int ac) {
    return func_00205A50(pkt, 0x1A, (unsigned long long)ac);
}
