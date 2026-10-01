// COMPILER: eegcc
// CFLAGS: -O2
// SDK (IOP sound RPC): sends a transfer request p = {channel, address, EE
// address, size}. Addresses and sizes above 2 MB (0x1FFFFF) and channels
// 16 and up are rejected with -1; otherwise command 0x48 goes to
// func_001157F0 with (channel << 24 | address, EE address, size) and 0 is
// returned.
extern void func_001157F0(int cmd, int a, int b, int c);

int func_0011A788(int *p) {
    int r = -1;

    if ((unsigned int)p[2] <= 0x1FFFFF && (unsigned int)p[3] <= 0x1FFFFF && p[0] < 0x10) {
        func_001157F0(0x48, (p[0] << 24) | p[1], p[2], p[3]);
        r = 0;
    }
    return r;
}
