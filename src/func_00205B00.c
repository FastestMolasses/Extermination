// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// GS packet helper: appends a TEX0_1 register write (A+D register 0x06) with
// func_00205A50: TBP0 | TBW<<14 | PSM<<20 | TW<<26 | TH<<30 | TCC<<34 |
// TFX<<35 | CBP<<37 | CPSM<<51 | CSM<<55 | CSA<<56 | CLD<<61. Returns the
// advanced packet pointer.
extern char *func_00205A50(char *pkt, int reg, unsigned long long value);

char *func_00205B00(char *pkt, unsigned int tbp0, unsigned int tbw, unsigned int psm,
                    unsigned int tw, unsigned int th, unsigned int tcc, unsigned int tfx,
                    unsigned int cbp, unsigned int cpsm, unsigned int csm, unsigned int csa,
                    unsigned int cld) {
    return func_00205A50(pkt, 6,
                         ((unsigned long long)cld << 61
                          | ((unsigned long long)csa << 56
                           | ((unsigned long long)csm << 55
                            | ((unsigned long long)cpsm << 51
                             | ((unsigned long long)cbp << 37
                              | ((unsigned long long)tfx << 35
                               | ((unsigned long long)tcc << 34
                                | ((unsigned long long)th << 30
                                 | ((unsigned long long)tw << 26
                                  | ((unsigned long long)psm << 20
                                   | ((unsigned long long)tbp0
                                    | (unsigned long long)tbw << 14))))))))))));
}
