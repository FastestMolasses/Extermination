// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Sound sequencer: handler for controller 0x63 events (called from the event
// dispatcher func_001152D8). Selects by the event's value byte (0x03):
//   0..15  - h5C = 2 and h60 = the value;
//   20     - loop start: h3A = byte 0x00, w14 = the current read offset w8,
//            h5C = 0, h5E = 0;
//   30     - loop end: when the loop count h5A is 0x7F (infinite) h38 = 1;
//            else if the counter h36 has reached h5A, w14, h38 and h36 are
//            cleared; else h36 is incremented and h38 = 1. Then h5C = 0;
//   16     - h5C = 1;
//   127    - h5C = 2, h60 = 0xFF.
// Every event advances the read offset w8 by 3.
// Fixes read from the original instructions: the case bodies are laid out in
// the order 0..15, 20, 30, 16, 127 (gcc keeps source order), and case 127
// stores h5C before h60.

typedef struct {
    unsigned char b0;       /* 0x00 */
    char pad_01[2];
    unsigned char b3;       /* 0x03 */
    int pad_04;
    int w8;                 /* 0x08 */
    char pad_0C[0x08];
    int w14;                /* 0x14 */
    char pad_18[0x1E];
    unsigned short h36;     /* 0x36 */
    unsigned short h38;     /* 0x38 */
    unsigned short h3A;     /* 0x3A */
    char pad_3C[0x1E];
    unsigned short h5A;     /* 0x5A */
    unsigned short h5C;     /* 0x5C */
    unsigned short h5E;     /* 0x5E */
    unsigned short h60;     /* 0x60 */
} S;

void func_00118C00(S *a0) {
    switch (a0->b3) {
    case 0 ... 15:
        a0->h5C = 2;
        a0->h60 = a0->b3;
        break;
    case 20:
        a0->h3A = a0->b0;
        a0->w14 = a0->w8;
        a0->h5C = 0;
        a0->h5E = 0;
        break;
    case 30:
        if (a0->h5A == 0x7F) {
            a0->h38 = 1;
        } else if (a0->h36 >= a0->h5A) {
            a0->w14 = 0;
            a0->h38 = 0;
            a0->h36 = 0;
        } else {
            a0->h36++;
            a0->h38 = 1;
        }
        a0->h5C = 0;
        break;
    case 16:
        a0->h5C = 1;
        break;
    case 127:
        a0->h5C = 2;
        a0->h60 = 0xFF;
        break;
    }
    a0->w8 += 3;
}
