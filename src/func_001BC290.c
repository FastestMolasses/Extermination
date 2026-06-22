// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Door articulation step + clip restart: advance the door clip one step
// (anim_advance_time(self, 1.0f) -> current time stored at blk+0xE). If the
// global transition gate D_008106B8 is clear, restart the clip from time 0
// (anim_clip_init(self,0,0,0)), clear self+0xB and return 1; otherwise return 0.
// Sibling of func_001BC240 (which commits the transition instead of gating).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 is wall #13 (clean-store branch lowering of the
// return-0 path -- 991202 reorders/duplicates the `addiu v0,1`/`paddub v0,0`
// trailing block and fills the beqz delay slot, capping at 90.71%). 2.3.3
// emits the byte-identical block (objdiff 100.0).
extern short anim_advance_time(void *self, float step);
extern void anim_clip_init(void *self, int a1, float f12, float f13);
extern unsigned char D_008106B8;

int func_001BC290(unsigned char *self, unsigned char *blk) {
    *(short *)(blk + 0xE) = anim_advance_time(self, 1.0f);
    if (D_008106B8 == 0) {
        anim_clip_init(self, 0, 0.0f, 0.0f);
        *(char *)(self + 0xB) = 0;
        return 1;
    }
    return 0;
}
