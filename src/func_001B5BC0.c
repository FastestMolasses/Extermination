// CFLAGS: -O4,p -sdatathreshold 4
// Unpack a 5-byte big-endian config record (selected by a0 within the table for
// the current set D_00275BD4) into the D_00810E5x/E6x/E7x global state words.
extern int D_00275BD4;
extern char *D_0028A6AC[2];
extern volatile short D_00810E70[8];
extern volatile short D_00810E72[8];
extern volatile unsigned short D_00810E74[8];
extern volatile unsigned short D_00810E76[8];
extern unsigned char D_00810E64[8];
extern unsigned char D_00810E65[8];
extern unsigned char D_00810E57[8];

void func_001B5BC0(int a0) {
    int set = D_00275BD4;
    char *rec = D_0028A6AC[0] + set * 9 * 2048 + a0 * 5;

    if (a0 == 0) {
        D_00810E72[0] = 0;
    } else {
        D_00810E72[0] = (short)((((unsigned char)rec[-5] << 8) | (unsigned char)rec[-4]) ^ 0xFFFF);
    }

    D_00810E70[0] = (short)((((unsigned char)rec[0] << 8) | (unsigned char)rec[1]) ^ 0xFFFF);
    D_00810E76[0] = (unsigned short)D_00810E74[0];
    D_00810E74[0] = (unsigned short)(~D_00810E72[0] & D_00810E70[0]);
    D_00810E64[0] = rec[2];
    D_00810E65[0] = rec[3];
    D_00810E57[0] = rec[4];
}
