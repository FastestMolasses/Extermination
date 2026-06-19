// CFLAGS: -O4,p -sdatathreshold 4
// D_00275B40 = D_00275B48 + 0x110  (advance a gp-rel cursor by 0x110).
extern int D_00275B48;
extern int D_00275B40;

void func_001CB5B0(void) {
    D_00275B40 = D_00275B48 + 0x110;
}
