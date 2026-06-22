// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Dispatch on b[4]: walks a->[0x18]->[0x18]->[0x18] (chained ptr deref)
// to reach object o; case 0 arms a state flag (o[0xB]=2) and bumps a
// counter (b[4]++) iff the flag is clear; case 1 returns 1 iff o[0xB]==3;
// all other paths (and the unmatched cases) return 0 via the shared tail.
int func_001BC6D0(int a, unsigned char *b) {
    unsigned char *o;
    o = (unsigned char *)*(int *)(*(int *)(*(int *)(a + 0x18) + 0x18) + 0x18);
    switch (b[4]) {
    case 0:
        if (o[0xB] == 0) {
            o[0xB] = 2;
            b[4] = b[4] + 1;
        }
        break;
    case 1:
        if (o[0xB] == 3) {
            return 1;
        }
        break;
    default:
        break;
    }
    return 0;
}
