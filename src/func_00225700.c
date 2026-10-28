// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Tail-call wrapper: one fixed GIF/AD packet through func_00207F80 on DMA
// index 1 with the packed corner pairs (0x7CD0, 0x8550) and (0x7D90, 0x85B0) and the
// colour word 0x80CE6000 (the 6th argument, in the second extra argument
// register). The same two calls appear inline in func_0022A020's per-mode
// branch (with 0x83C0/0x8420 there instead of 0x8550/0x85B0).
extern void func_00207F80(int, int, int, int, int, int);

void func_00225700(void) {
    func_00207F80(1, 0x7CD0, 0x8550, 0x7D90, 0x85B0, 0x80CE6000);
}
