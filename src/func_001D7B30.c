// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Table search: returns a pointer to the first 0x78-byte entry in the table at
// D_00251C50 whose first word equals a 16-bit key. The key is 0xF00 if
// func_001D2910(8) is nonzero, otherwise the big-endian 16-bit value formed from
// the two bytes at D_00810700/D_00810701. Scans up to 0x2D (45) entries; if no
// entry matches, falls back to returning the table base (&D_00251C50).
extern int func_001D2910(int);
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int D_00251C50[];

int *func_001D7B30(void) {
    unsigned int i;
    int *p;
    int key;

    if (func_001D2910(8) != 0) {
        key = 0xF00;
    } else {
        key = ((int)D_00810700 << 8) + (int)D_00810701;
    }

    p = D_00251C50;
    i = 0;
    do {
        if (*p == key) {
            return p;
        }
        i++;
        p = (int *)((char *)p + 0x78);
    } while (i < 0x2D);

    return D_00251C50;
}
