// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// GS packet helper: appends a TRXDIR register write (A+D register 0x53) with
// func_00205A50, value xdir (zero-extended). Returns the advanced packet
// pointer.
extern char *func_00205A50(char *pkt, int reg, unsigned long long value);

char *func_00205F40(char *pkt, unsigned int xdir) {
    return func_00205A50(pkt, 0x53, (unsigned long long)xdir);
}
