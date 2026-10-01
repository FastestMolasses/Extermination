// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Movie decoder: initialises the IPU input ring r. r+0 = base, r+4 = the
// DMA tag address (low 28 bits of tagaddr, uncached-accelerated 0x20000000),
// r+8 = the sector count and r+0x18 its byte size (sectors * 2048), r+0x50 /
// r+0x54 the two callback words; creates the ring's semaphore (max and
// initial count 1) into r+0x40, resets the ring (func_00204250) and clears
// the 64-bit word r+0x48. Returns 1.
typedef struct SemaParam {
    int count;
    int max_count;
    int init_count;
    int wait_threads;
    unsigned int attr;
    unsigned int option;
} SemaParam;

extern int CreateSema(SemaParam *p);
extern void func_00204250(int *r);

int func_002041D0(int *r, int base, unsigned long long tagaddr, int sectors, int cb0, int cb1) {
    SemaParam sp;

    r[0] = base;
    r[1] = (unsigned int)(tagaddr & 0x0FFFFFFF) | 0x20000000;
    r[2] = sectors;
    r[6] = sectors << 11;
    r[0x14] = cb0;
    r[0x15] = cb1;
    sp.init_count = 1;
    sp.max_count = 1;
    r[0x10] = CreateSema(&sp);
    func_00204250(r);
    *(long long *)(r + 0x12) = 0;
    return 1;
}
