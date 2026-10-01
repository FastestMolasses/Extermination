// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// GS packet helper: appends a TEST_1 register write (A+D register 0x47) with
// func_00205A50: ATE | ATST<<1 | AREF<<4 | AFAIL<<12 | DATE<<14 | DATM<<15 |
// ZTE<<16 | ZTST<<17. Returns the advanced packet pointer.
extern char *func_00205A50(char *pkt, int reg, unsigned long long value);

char *func_00205D40(char *pkt, unsigned int ate, unsigned int atst, unsigned int aref,
                    unsigned int afail, unsigned int date, unsigned int datm, unsigned int zte,
                    unsigned int ztst) {
    return func_00205A50(pkt, 0x47,
                         ((unsigned long long)ztst << 17
                          | ((unsigned long long)zte << 16
                           | ((unsigned long long)datm << 15
                            | ((unsigned long long)date << 14
                             | ((unsigned long long)afail << 12
                              | ((unsigned long long)aref << 4
                               | ((unsigned long long)ate
                                | (unsigned long long)atst << 1))))))));
}
