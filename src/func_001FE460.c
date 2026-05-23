// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int func_001FE460(int *a0) {
    int v1 = a0[0];
    int v0 = a0[2];
    int *p = (int *)((char *)a0 + v1);
    int *q = (int *)((char *)p + v0);
    return q[1];
}
