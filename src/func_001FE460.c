// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int func_001FE460(int *a0) {
    int v0 = a0[2];
    int v1 = a0[0];
    v1 = (int)a0 + v1;
    v0 = v0 + v1;
    return *(int *)(v0 + 4);
}
