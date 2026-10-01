// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// GS packet helper: appends a PRIM register write (A+D register 0x00) with
// func_00205A50: PRIM | IIP<<3 | TME<<4 | FGE<<5 | ABE<<6 | AA1<<7 | FST<<8 |
// CTXT<<9 | FIX<<10. Returns the advanced packet pointer.
extern char *func_00205A50(char *pkt, int reg, unsigned long long value);

char *func_00205BC0(char *pkt, unsigned int prim, unsigned int iip, unsigned int tme,
                    unsigned int fge, unsigned int abe, unsigned int aa1, unsigned int fst,
                    unsigned int ctxt, unsigned int fix) {
    return func_00205A50(pkt, 0,
                         ((unsigned long long)fix << 10
                          | ((unsigned long long)ctxt << 9
                           | ((unsigned long long)fst << 8
                            | ((unsigned long long)aa1 << 7
                             | ((unsigned long long)abe << 6
                              | ((unsigned long long)fge << 5
                               | ((unsigned long long)tme << 4
                                | ((unsigned long long)prim
                                 | (unsigned long long)iip << 3)))))))));
}
