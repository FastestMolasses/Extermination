// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Reads part of a disc file into buf. f+0 is the file's first sector, f+4 its
// size in bytes; offset is a byte offset (whole sectors are used) and size
// the byte count (negative: the whole file). The sector count is rounded up;
// the read (func_00112440, sceCdRead, mode: trycount 0, spindle control 1,
// data pattern 0) is retried after each func_00113280(0) (sceCdSync) until it
// is accepted. Returns the byte count read (sectors * 2048).
typedef struct CdMode {
    unsigned char trycount;
    unsigned char spindlctrl;
    unsigned char datapattern;
    unsigned char pad;
} CdMode;

extern int func_00112440(int lsn, unsigned int sectors, void *buf, CdMode *mode);
extern int func_00113280(int mode);

int func_00200780(int *f, void *buf, int offset, int size) {
    CdMode mode;
    unsigned int sectors;
    int first;

    mode.trycount = 0;
    mode.spindlctrl = 1;
    mode.datapattern = 0;
    if (size < 0) {
        sectors = (unsigned int)(f[1] + 0x7FF) >> 11;
    } else {
        sectors = (size + 0x7FF) >> 11;
    }
    first = offset >> 11;
    func_00113280(0);
    while (func_00112440(f[0] + first, sectors, buf, &mode) == 0) {
        func_00113280(0);
    }
    return sectors << 11;
}
