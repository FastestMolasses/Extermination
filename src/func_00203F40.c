// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// 8-arg block-copy region dispatcher: validates total length then routes the copy across three branch arms via block_copy().
int block_copy();

int func_00203F40(int a0, unsigned int a1, int a2, unsigned int a3,
                  int t0, unsigned int t1, unsigned int t2, unsigned int t3) {
    if ((a1 + a3) < (t1 + t3)) {
        return 0;
    }
    if (t1 >= a1) {
        block_copy(a0, t0, a1);
        block_copy(a2, t0 + a1, t1 - a1);
        block_copy((a2 + t1) - a1, t2, t3);
    } else if (t3 >= (a1 - t1)) {
        block_copy(a0, t0, t1);
        block_copy(a0 + t1, t2, a1 - t1);
        block_copy(a2, (t2 + a1) - t1, t3 - (a1 - t1));
    } else {
        block_copy(a0, t0, t1);
        block_copy(a0 + t1, t2, t3);
    }
    return t1 + t3;
}
