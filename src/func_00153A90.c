// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Active (hurt-gated) state of the phase-gated boss-like enemy behind
// func_00153950 (see FINDINGS "DAMAGE/CONSUMPTION"). Polls the +0x36 damage
// mailbox: nonzero hit and the vulnerability global D_0081077B == 0 -> absorb
// (ping +0x00 = 1, clear the mailbox +0x36); otherwise enter the hurt sub-state
// (+0x04 = 2, +0x05 = 0). Then, unless the enemy is mid-invulnerable-phase
// (D_0081077B == 1 with the kill flag D_008107FB still clear), run the common
// per-tick update: advance the animation clip (anim_advance_time(self, 1.0f),
// result published to *arg1), func_00131ED0, func_001C68C0 (in-place skeletal
// anim), func_001B17A0 (visibility cull / transform), then the per-actor vtable
// hook at +0x4C.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the lone residual is wall #13 (the clean-store delay-slot nop -- 991202
// fills the slot, CodeWarrior 2.3.3 leaves the nop). The 2.3.3 build leaves it,
// so this readable C is byte-identical. Verified objdiff 100.0% vs
// build/expected/func_00153A90.o (991202 = 97.87%).
extern int anim_advance_time(char *self, float step);
extern void func_00131ED0(char *);
extern void func_001C68C0(char *);
extern void func_001B17A0(char *);
extern unsigned char D_0081077B;
extern unsigned char D_008107FB;

void func_00153A90(char *arg0, int *arg1) {
    if (*(short *)(arg0 + 0x36) != 0) {
        if (D_0081077B == 0) {
            *(char *)(arg0 + 0) = 1;
            *(short *)(arg0 + 0x36) = 0;
        } else {
            *(char *)(arg0 + 4) = 2;
            *(char *)(arg0 + 5) = 0;
        }
    }
    if (D_0081077B != 1 || D_008107FB != 0) {
        *arg1 = anim_advance_time(arg0, 1.0f);
        func_00131ED0(arg0);
        func_001C68C0(arg0);
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
    }
}
