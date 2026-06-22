// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Spawn-and-initialize a class-0xC actor. Allocates via func_001AFA90(0xC) and,
// on success, fills the new actor: type byte +0x03 = 0xC, sub-byte +0x0D = 0x63,
// short field +0x94 = arg1, owner/handle +0x40 = arg0, behavior +0x10 =
// func_001C7EB0, counter +0x24 = 0, float +0x2E8 = 0.5f (0x3F000000 stored as
// int), param +0x2EC = arg2. Returns void.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 991202
// diverges in register allocation / store scheduling on the clean-store idiom
// (72.9%). The 2.3.3 build matches the target's regalloc and store order, so this
// readable C is byte-identical. Verified objdiff 100% vs
// build/expected/func_001C8140.o.
extern char *func_001AFA90(int);
extern int func_001C7EB0;

void func_001C8140(int arg0, short arg1, int arg2) {
    char *v0;

    v0 = func_001AFA90(0xC);
    if (v0 != 0) {
        *(char *)(v0 + 3) = 0xC;
        *(char *)(v0 + 0xD) = 0x63;
        *(short *)(v0 + 0x94) = arg1;
        *(int *)(v0 + 0x40) = arg0;
        *(int *)(v0 + 0x10) = (int)&func_001C7EB0;
        *(int *)(v0 + 0x24) = 0;
        *(int *)(v0 + 0x2E8) = 0x3F000000;
        *(int *)(v0 + 0x2EC) = arg2;
    }
}
