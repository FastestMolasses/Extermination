// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Non-leaf GIF/DMA-tag builder: acquires a block via func_001CB5F0, zero-clears
// the first quadword (sq $zero), then writes GIFtag/qword fields. The 3rd caller
// arg (saved across the call) is stored as a 64-bit value at +0x20.
typedef unsigned __int128 uint128;
extern unsigned char *func_001CB5F0(int a0, int a1, int a2, int a3);

unsigned char *func_001CB950(int a0, int a1, long a2, int a3) {
    unsigned char *p = func_001CB5F0(a0, a1, 3, a3);
    *(uint128 *)(p + 0x0) = 0;
    *(int *)(p + 0xC) = 0x50000002;
    *(long *)(p + 0x10) = 0x1000000000008001;
    *(long *)(p + 0x18) = 0xE;
    *(long *)(p + 0x20) = a2;
    *(long *)(p + 0x28) = 6;
    return p;
}
