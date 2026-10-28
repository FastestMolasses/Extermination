// COMPILER: eegcc
// CFLAGS: -O2
// newlib _lseek_r: clear errno (D_008234E0), call the _lseek syscall wrapper
// func_0010BE60; if it fails (-1) with errno set, store errno into the reent
// block's first word (_errno). Returns the 64-bit offset (_off_t is long).
extern int D_008234E0;
extern long func_0010BE60(int fd, long pos, int whence);

long func_00120B38(int *ptr, int fd, long pos, int whence) {
    long r;
    D_008234E0 = 0;
    r = func_0010BE60(fd, pos, whence);
    if (r == -1) {
        if (D_008234E0 != 0) {
            *ptr = D_008234E0;
        }
    }
    return r;
}
