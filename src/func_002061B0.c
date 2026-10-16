// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Movie display-gate open. Spins on func_001009C8(0) (the GS CSR FIELD bit)
// while it still equals `field`, so func_002061B0(1) returns on the first
// field-0 vblank; then raises the gate D_00275C80, clears D_007A55F8[0] and
// D_00275C7C. The two gp-relative flags are volatile (as in func_00206030):
// that pins the three stores in source order, the original's order.
extern volatile int D_00275C80;
extern volatile int D_00275C7C;
extern int D_007A55F8[2];
extern int func_001009C8(int);

void func_002061B0(int field) {
    do {
    } while (func_001009C8(0) == field);
    D_00275C80 = 1;
    D_007A55F8[0] = 0;
    D_00275C7C = 0;
}
