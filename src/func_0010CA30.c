// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): ring-buffer consume — advance read ptr, wrap at end.
struct ring {
    /* 0x0 */ int size;
    /* 0x4 */ int count;
    /* 0x8 */ char *rd;
};

void func_0010CA30(struct ring *r) {
    char *next;
    int one = 1;
    r->count = r->count - one;
    next = r->rd + 1;
    r->rd = next;
    if (next == (char *)r + (r->size + 0x10)) {
        r->rd = (char *)r + 0x10;
    }
}
