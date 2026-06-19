// COMPILER: eegcc
// CFLAGS: -O2
// State machine on field 0x5C; updates 0x5A/0x5E and advances counter 0x08 by 3.
typedef struct {
    char pad3[3];
    unsigned char b3;    // 0x03
    int pad4[1];
    int w8;              // 0x08
    char pad_c[0x5A - 0x0C];
    short h5A;           // 0x5A
    unsigned short h5C;  // 0x5C
    short h5E;           // 0x5E
} S;

void func_00118BA0(S *a0) {
    switch (a0->h5C) {
    case 0:
        a0->h5E = 0;
        a0->h5A = a0->b3;
        break;
    case 1:
        a0->h5E = a0->b3;
        break;
    case 2:
        a0->h5E = a0->b3;
        break;
    default:
        break;
    }
    a0->w8 += 3;
}
