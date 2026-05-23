// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
// Stores a0 to D_00275670[0x98/4] (field at offset 0x98 of the struct pointed to by D_00275670).
int *D_00275670;
void func_001D2580(int a0) {
    D_00275670[0x98 / 4] = a0;
}
