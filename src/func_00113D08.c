// COMPILER: eegcc
// CFLAGS: -O2
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.39% NEARMISS).
// Fixes read from the original instructions:
//  - the last-error local is set whenever a chunk read reports an error, not only
//    when debug printing is on (the copy runs on both sides of the debug check);
//  - the retry loop is a do/while that ends when all sectors are read or when an
//    error comes back with zero sectors transferred (no goto-into-loop shape);
//  - both loop counters are zeroed after the "ready" check, error first, then the
//    sector count (this order fixes the saved-register colouring);
//  - callee func_00113F68 returns int (its definition), result split into
//    sector count (low 16 bits) and error code (high 16 bits).
//
// Sector read with retries: prints a debug trace when D_00241CF8 > 0, returns 0
// if the reader is not ready (D_00241D40 == 0), primes the transfer with
// func_0010DFD8(buf, nsec * 2048), then either reads in a loop (mode != 0),
// advancing the buffer by 2048 bytes per sector, or issues one read. The last
// error is stored to *err; returns the number of sectors read.
extern int D_00241CF8;
extern int D_00241D40;
extern unsigned char D_00241D50[];
extern char D_0026BDC8[];
extern char D_0026BDF8[];
extern char D_0026BE40[];
extern int func_00122B58(char *fmt, ...);
extern void func_0010DFD8(int a, int b);
extern int func_00113F68(int a0, int a1, int a2, int a3, unsigned char *t0);

int func_00113D08(int nsec, int buf, int mode, int *err) {
    int done;
    int lasterr;
    int n;
    int e;
    unsigned int r;

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BDC8, nsec);
    }
    if (D_00241D40 == 0) {
        return 0;
    }
    lasterr = 0;
    done = 0;
    func_0010DFD8(buf, nsec << 11);
    if (mode != 0) {
        do {
            r = func_00113F68(0, nsec - done, buf + (done << 11), 2, D_00241D50);
            n = r & 0xFFFF;
            e = r >> 16;
            done += n;
            if (e != 0) {
                lasterr = e;
                if (D_00241CF8 > 0) {
                    func_00122B58(D_0026BDF8, done, n, nsec, e);
                }
            }
        } while (done != nsec && (e == 0 || n != 0));
        if (D_00241CF8 > 0) {
            func_00122B58(D_0026BE40);
        }
        *err = lasterr;
    } else {
        r = func_00113F68(0, nsec, buf, 2, D_00241D50);
        e = r >> 16;
        done = r & 0xFFFF;
        *err = e;
    }
    return done;
}
