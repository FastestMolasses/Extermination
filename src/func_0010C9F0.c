// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): ring buffer write-slot advance with wrap.
struct ring {
    int size;       // 0x0
    int count;      // 0x4
    int read;       // 0x8
    char *write;    // 0xC
};

void func_0010C9F0(struct ring *r) {
    r->count++;
    r->write++;
    if (r->write == (char *)r + 0x10 + r->size) {
        r->write = (char *)r + 0x10;
    }
}
