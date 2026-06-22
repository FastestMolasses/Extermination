// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Audio/channel arbiter init: silences voice channels 0 and 1 via
// func_0011A8C8(ch, 0), then sets per-channel volume/pan via
// func_0011A918(ch, l, r) — full-pan defaults (0x7FFF/0) when the mode byte
// D_0081011C is clear, centered (0x3000/0x3000) when set — finishes with
// func_0011A848(3), and latches state 2 into the actor (*arg0 = 2).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the post-store/branch scheduling diverges (87.5%); 2.3.3 is
// byte-identical (objdiff 100.0 vs build/expected/func_00206520.o).
extern void func_0011A8C8(int, int);
extern void func_0011A918(int, int, int);
extern void func_0011A848(int);
extern unsigned char D_0081011C;

void func_00206520(int *arg0) {
    func_0011A8C8(0, 0);
    func_0011A8C8(1, 0);
    if (D_0081011C == 0) {
        func_0011A918(1, 0x7FFF, 0);
        func_0011A918(2, 0, 0x7FFF);
    } else {
        func_0011A918(1, 0x3000, 0x3000);
        func_0011A918(2, 0x3000, 0x3000);
    }
    func_0011A848(3);
    *arg0 = 2;
}
