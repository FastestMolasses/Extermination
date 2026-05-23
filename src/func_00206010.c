// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
void func_00206010(volatile int *a0) {
    if (a0[3] <= 0) {
        return;
    }
    a0[3]--;
}
