// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
// It is the _lseek stub called by _lseek_r (func_00120B38) with (fd, pos,
// whence); it ignores them and always fails with -1 (64-bit offset result).
long func_0010BE60(int fd, long pos, int whence) {
    return -1;
}
