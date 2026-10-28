// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Initialises a record: clears the 12-byte header block (arg0) and the 0x2C-byte
// record (arg1) with func_00121A28 (memset-like: pointer, value, size), stores
// the buffer (5th argument) and the two parameters a/b, sets the two bytes at
// +0x24/+0x25 to 0x80, then calls func_001108E8(a, b, buf) (re-reading the
// stored buffer). Finally clears flagsA[0..5] (+0x18), sets flagsB[0] = 0,
// flagsB[1] = 1 and flagsB[2..5] = 0xFF (+0x1E).
typedef struct {
    unsigned char *buf;         /* 0x00 */
    int a;                      /* 0x04 */
    int b;                      /* 0x08 */
    char pad0C[0x0C];           /* 0x0C */
    unsigned char flagsA[6];    /* 0x18 */
    unsigned char flagsB[6];    /* 0x1E */
    unsigned char r;            /* 0x24 */
    unsigned char g;            /* 0x25 */
    char pad26[6];              /* 0x26 */
} Rec5860;

extern void func_00121A28(void *p, int value, int size);
extern int func_001108E8(int a, int b, unsigned char *buf);

void func_001B5860(void *hdr, Rec5860 *rec, int a, int b, unsigned char *buf) {
    int i;

    func_00121A28(hdr, 0, 0xC);
    func_00121A28(rec, 0, 0x2C);
    rec->buf = buf;
    rec->a = a;
    rec->b = b;
    rec->r = 0x80;
    rec->g = 0x80;
    func_001108E8(a, b, rec->buf);
    for (i = 0; i < 6; i++) {
        rec->flagsA[i] = 0;
    }
    rec->flagsB[0] = 0;
    rec->flagsB[1] = 1;
    for (i = 2; i < 6; i++) {
        rec->flagsB[i] = 0xFF;
    }
}
