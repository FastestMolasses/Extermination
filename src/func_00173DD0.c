// NEARMISS func_00173DD0  (vram 0x00173DD0, 0x8C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.69% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// reloc-pair interleave: CW materializes &D_002486F0 as split lui/addiu/lq (full GPR address before the load); mwcc 2.3.3 folds %lo into the lq (lui at; lq v1,%lo(at)) for the single-use address — robust across bare extern, pointer-stmt, array[1], forced-GPR forms. The fold shifts downstream FP reg...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS (FINDINGS "KNIFE/MELEE DECODED"): heavy-stab in-swing yaw steer
// (knife SQUARE attack, mode 0x22 states 2/3): while the stab swings, steer the
// player yaw +0xC4 toward the goal heading +0x218 at pi * D_002486F0[gait] / 180
// rad/tick (D_002486F0 = {0,0.5,1.0,2.0}f rate table indexed by gait byte
// +0x23F; the quadword table is copied to the stack first), gated on
// func_00174AC0(self,2); the steer helper is func_001B12B0(goal,cur,rate)->yaw.
//
// 2.3.3 CRACKED the prior FP-stall scheduling wall (the explicit div.s; nop;
// nop; jal padding now matches byte-exact). The SOLE residual is the documented
// reloc-pair-interleave artifact: CW materializes &D_002486F0 split as
// `lui v0,%hi; addiu v0,v0,%lo; lq v0,0(v0)` (full GPR address, dest addr
// addiu v1,sp,0x20 after the lq); mwcc folds the %lo into the load
// (`lui at,%hi; lq v1,%lo(at)`) for the single-use address in every form tried
// (bare extern, pointer-statement idiom 12a, array[1], idiom-7 forced GPR). The
// fold shifts downstream FP register coloring (fv1/fv0f vs ft0/fv1) but the
// body and control flow are byte-faithful. mwcc treats the reloc pair
// atomically; needs a saved-reg/second-live-temp address pool-order datum.
typedef unsigned __int128 u128;
extern u128 D_002486F0;
extern int func_00174AC0(unsigned char *p, int n);
extern float func_001B12B0(float goal, float cur, float rate);

void func_00173DD0(unsigned char *self) {
    float rates[4];
    *(u128 *)rates = D_002486F0;
    if (func_00174AC0(self, 2)) {
        *(float *)(self + 0xC4) = func_001B12B0(
            *(float *)(self + 0x218), *(float *)(self + 0xC4),
            (3.1415927410125732f * rates[self[0x23F]]) / 180.0f);
    }
}
