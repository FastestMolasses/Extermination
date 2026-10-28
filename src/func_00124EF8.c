// COMPILER: eegcc
// CFLAGS: -O2
// newlib _write_r: clear errno (D_008234E0), call the _write syscall wrapper;
// if it fails (-1) with errno set, store errno into the reent block's first
// word (_errno). Returns the byte count (_ssize_t, a 64-bit long).
extern int D_008234E0;
extern int sub__9PsIIlibkernl2000(int fd, char *buf, int len);

long func_00124EF8(int *ptr, int fd, const void *buf, unsigned int cnt) {
    long r;
    D_008234E0 = 0;
    r = sub__9PsIIlibkernl2000(fd, (char *)buf, cnt);
    if (r == -1) {
        if (D_008234E0 != 0) {
            *ptr = D_008234E0;
        }
    }
    return r;
}
