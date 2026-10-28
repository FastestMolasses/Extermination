// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.75% NEARMISS).
// Fixes read from the original instructions:
// - The argument is a record (two byte counts, two destination pointers, two inline
//   0x40-byte source buffers at +0x10 / +0x50); indexing the buffer member directly
//   lets the loop optimiser hoist the buffer base into the loop preheader, as the
//   original does, instead of a hand-hoisted source pointer that the scheduler pulled
//   into the guard's delay slot.
//
// Copies the two pending byte payloads out of an uncached (arg | 0x20000000) view of
// the reply record into their destination buffers, each guarded by count > 0 with the
// count re-read from the record every iteration, then tail-calls func_001115D0 with
// &D_00241D48.

extern int D_00241D48;
extern void func_001115D0(int *p);

struct reply_buf {
    int n0;                  /* +0x00 bytes to copy into d0 */
    int n1;                  /* +0x04 bytes to copy into d1 */
    unsigned char *d0;       /* +0x08 */
    unsigned char *d1;       /* +0x0C */
    unsigned char b0[0x40];  /* +0x10 */
    unsigned char b1[0x40];  /* +0x50 */
};

void func_00111818(void *arg) {
    struct reply_buf *p = (struct reply_buf *)((unsigned int)arg | 0x20000000);
    unsigned char *dst;
    int i;

    if (p->n0 > 0) {
        dst = p->d0;
        for (i = 0; i < p->n0; i++) {
            dst[i] = p->b0[i];
        }
    }
    if (p->n1 > 0) {
        dst = p->d1;
        for (i = 0; i < p->n1; i++) {
            dst[i] = p->b1[i];
        }
    }
    func_001115D0(&D_00241D48);
}
