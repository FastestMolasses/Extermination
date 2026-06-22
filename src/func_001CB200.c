// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Spawns/inits a class-6 object: func_001D8C20(6), then func_001C7420(arg0,
// 0x3F5, 3) and func_001D4640(arg1, arg0+0x80). The global object pointed to
// by D_00275670 holds a bump-allocated command buffer at field +0x1C: the
// current cursor is read once into temp_s0 (passed to func_001CAAC0 with the
// D_00275B44+0xB0 area pointer), then a 0x10-byte command record is written
// at the cursor (+3 byte = 0x60, +4 word = 0, +0 half = 0) and the cursor is
// advanced by 0x10. Closes with func_001D8C20(0).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 build
// (which scores 88.27%): residual under 991202 is the idiom-13 delay-slot
// fill family. Verified TRUE objdiff 100.0 byte-identical vs
// build/expected/func_001CB200.o.
//
// D_00275670 / D_00275B44 are gp-relative small-data (sdatathreshold 8).
// The global object base (D_00275670) is read once into `base` so mwcc keeps
// it in a register and reloads only the +0x1C cursor field per store.
extern int func_001C7420(int, int, int);
extern int func_001D4640(int, int);
extern int func_001D8C20(int);
extern int func_001CAAC0(int, char *);
extern char *D_00275670;
extern int D_00275B44;

void func_001CB200(int arg0, int arg1) {
    char *base;
    char *temp_s0;

    temp_s0 = *(char **)(D_00275670 + 0x1C);
    func_001D8C20(6);
    func_001C7420(arg0, 0x3F5, 3);
    func_001D4640(arg1, arg0 + 0x80);
    base = D_00275670;
    *(char *)(*(char **)(base + 0x1C) + 3) = 0x60;
    *(int *)(*(char **)(base + 0x1C) + 4) = 0;
    *(short *)(*(char **)(base + 0x1C) + 0) = 0;
    *(char **)(base + 0x1C) = *(char **)(base + 0x1C) + 0x10;
    func_001CAAC0(D_00275B44 + 0xB0, temp_s0);
    func_001D8C20(0);
}
