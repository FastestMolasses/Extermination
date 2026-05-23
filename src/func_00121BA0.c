// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
void func_00121BA0(int *a0, int *a1) {
    int v0;
    int *a0p;
    int v1;
    if (a1 == 0) {
        return;
    }
    v0 = a1[1];
    a0p = (int *)a0[0x4C / 4];
    v0 = (int)((char *)a0p + v0 * 4);
    v1 = *(int *)v0;
    a1[0] = v1;
    *(int *)v0 = (int)a1;
}
