// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Builds a host-to-GS image transfer packet (7 quadwords) at pkt: a zero
// DMA quadword whose upper words are VIF FLUSH (0x11000000) and VIF
// DIRECT for qwc + 6 quadwords; a GIF A+D tag (NLOOP 4) with BITBLTBUF
// (DBP, DBW, DPSM), TRXPOS (DSAX, DSAY), TRXREG (RRW, RRH) and TRXDIR = 0
// (host to local); then the IMAGE GIF tag for qwc quadwords (EOP).
typedef unsigned int u128 __attribute__((mode(TI)));

void func_001CCE80(char *pkt, int dbp, int dbw, int dpsm, int dsax, int dsay,
                   int rrw, int rrh, int qwc) {
    long long bitbltbuf = (long long)dbp << 32 | (long long)dbw << 48 | (long long)dpsm << 56;
    long long trxpos = (long long)dsax << 32 | (long long)dsay << 48;
    long long trxreg = (long long)rrw | (long long)rrh << 32;

    *(u128 *)pkt = 0;
    *(int *)(pkt + 0x08) = 0x11000000;
    *(int *)(pkt + 0x0C) = (qwc + 6) | 0x50000000;
    *(long long *)(pkt + 0x10) = 4 | (long long)0x10000000 << 32;
    *(long long *)(pkt + 0x18) = 0xE;
    *(long long *)(pkt + 0x20) = bitbltbuf;
    *(long long *)(pkt + 0x28) = 0x50;
    *(long long *)(pkt + 0x30) = trxpos;
    *(long long *)(pkt + 0x38) = 0x51;
    *(long long *)(pkt + 0x40) = trxreg;
    *(long long *)(pkt + 0x48) = 0x52;
    *(long long *)(pkt + 0x50) = 0;
    *(long long *)(pkt + 0x58) = 0x53;
    *(long long *)(pkt + 0x60) = qwc | (0x8000 | (long long)0x08000000 << 32);
    *(long long *)(pkt + 0x68) = 0;
}
