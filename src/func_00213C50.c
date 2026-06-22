// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Wrap-around byte copy: builds a 9-byte window at offset 0x90 from a ring
// of bytes starting at offset 0x50, indexed by p[0x19], length p[0x18].
// When sel==1 the start index is rolled back by one (with wrap to len-1).
// NOTE: requires 'i' declared before 'idx' (regalloc: idx->a3, i->a2) and the
// idx==0-first conditional (to emit the bnezl branch-likely). 991202 caps at
// 91.35 (it mis-lowers the decrement to beqz/nop/b); 2.3.3 emits the bnezl.
void func_00213C50(unsigned char *p, int sel) {
    int i;
    int idx;
    idx = p[0x19];
    if (sel == 1) {
        if (idx == 0) { idx = p[0x18] - 1; }
        else { idx -= 1; }
    }
    for (i = 0; i < 9; i++) {
        p[0x90 + i] = p[0x50 + idx];
        idx += 1;
        if (idx >= (int)p[0x18]) { idx = 0; }
    }
}
