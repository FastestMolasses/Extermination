// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// State-init/reset helper on an actor-or-state struct (unsigned char *):
// stamps fixed constants into several flag/counter bytes (+0x5=5, +0x6=0,
// +0x1F0=11, +0x25F=2), then if the +0x236 event byte is set, masks the
// +0x235 flag to its low bit and clears +0x236.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 was wall #13 — it fills the `beqz` delay slot (and
// reschedules `li v1,11` ahead of `sb zero,6(a0)`) where CodeWarrior 2.3.3
// leaves the clean store ordering + nop. The 2.3.3 build is byte-identical
// (.text cmp vs build/expected) → readable plain C, no asm void.
void func_00179680(unsigned char *arg0) {
    arg0[0x5] = 5;
    arg0[0x6] = 0;
    arg0[0x1F0] = 11;
    arg0[0x25F] = 2;
    if (arg0[0x236] != 0) {
        arg0[0x235] = arg0[0x235] & 1;
        arg0[0x236] = 0;
    }
}
