// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Actor sub-state tick wrapper: if the actor's anim block (blk) has its
// "active" flag set (signed byte at blk+0xC != 0), advance its articulation
// clip one step (1.0f) and store the resulting current-time (short) at blk+0xE.
// Then run the per-actor update func_001BA1F0(self) and return whether it
// reported a non-zero result (1) or not (0).
//
// Matched with mwcc 2.3.3 (mwccps2-2.3.3-000906), not the pinned 991202: the
// signed `lb blk+0xC` is read; the residual under 991202 is the clean-store
// delay-slot wall. 2.3.3 is byte-identical (objdiff 100%).
extern short anim_advance_time(void *self, float step);
extern int func_001BA1F0(void *self);

int func_001BC0E0(unsigned char *self, unsigned char *blk) {
    if (*(char *)(blk + 0xC) != 0) {
        *(short *)(blk + 0xE) = anim_advance_time(self, 1.0f);
    }
    if (func_001BA1F0(self) != 0) {
        return 1;
    }
    return 0;
}
