// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-sum checksum over a fixed 0x640-byte buffer; returns 1 if the low 8
// bits of the running sum are nonzero, else 0. The init-to-zero of sum and the
// 0/1 result both materialize via the mwcc paddub (128-bit clear) idiom; the
// inverted ternary (== 0 ? 0 : 1) is required so 2.3.3 emits the
// paddub-zero + addiu-1 + movn select rather than a movz with negated logic.
int func_00228C50(unsigned char *buf) {
    unsigned int i;
    int sum;

    sum = 0;
    for (i = 0; i < 0x640U; i++) {
        sum = (sum + buf[i]) & 0xFF;
    }
    return (sum & 0xFF) == 0 ? 0 : 1;
}
