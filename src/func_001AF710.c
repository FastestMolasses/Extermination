// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Free-list initializer: zeroes 0x480 records of 13 qwords (0xD0 bytes) each via sq,
// records the base pointer of each into the D_007D4640[] slot array, then publishes the
// array head (D_00275BD0) and the count (D_00275BCC = 0x480).
typedef unsigned __int128 s128;

extern short D_00275BCC;
extern s128 **D_00275BD0;
extern s128 *D_007D4640[0x480];
extern s128 D_007D5840;

void func_001AF710(void) {
    s128 *q;
    int i;
    int j;
    s128 *rec;
    s128 **slot;

    rec = &D_007D5840;
    slot = D_007D4640;
    i = 0;
    do {
        j = 0xD;
        q = rec;
        do {
            *q = 0;
            j -= 1;
            q += 1;
        } while (j != 0);
        i += 1;
        *slot = rec;
        rec += 0xD;
        slot += 1;
    } while (i < 0x480);
    D_00275BD0 = D_007D4640;
    D_00275BCC = 0x480;
}
