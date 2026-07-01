// NEARMISS func_001C02E0  (vram 0x001C02E0, 0x3F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.51% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Cluster of genuine mwcc-vs-CW artifacts, logic/structure fully recovered: (1) CW emits an unreachable duplicate copy of the 'alloc-failed -> self+4=3; return' arm inline right after the success arm's tail jump (documented dead-trailing-return-block lowering wall), shifting every later branch targ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// objdiff 94.51% via mwcc 2.3.3 (mwcps2-2.3.3-000906); pinned 991202 build
// reaches 88.15%. LOGIC and STRUCTURE are fully recovered (verified against
// the .s instruction-by-instruction: every call, store, and branch target
// matches). Residual is a cluster of genuine mwcc-vs-CW scheduling/dead-code
// artifacts, not a source defect:
//  - One unreachable duplicate block: CW emits a second dead copy of the
//    "alloc failed -> self+4=3; return" arm inline right after the success
//    arm's own tail jump, before the real (reachable) copy further down
//    (the documented "dead trailing return-0 block" mwcc-vs-CW lowering
//    wall) -- this alone shifts every following branch target by 0x10,
//    cascading into several DIFF_ARG_MISMATCH-on-branch-offset entries.
//  - Several call/store delay-slot fills where CW leaves a `nop` (or hoists
//    an unrelated load a instruction earlier) and mwcc233 schedules
//    differently: func_001AFA90(2,2)'s first-arg copy vs the D_700038AC
//    store ordering, a `beqz` following func_001BF630's call result (CW
//    nop, mwcc233 fills from later code), and a redundant re-copy of
//    a0=self CW performs before func_001C6380 that mwcc233 elides as dead
//    (already live in a0). Both idiom-13 (clean-store nop) and idiom-20
//    (branch-likely dispatch) were applied surgically already: the +5
//    sub-state re-load (not reusing the switch scrutinee) was required to
//    stop mwcc233 from branch-likely-ing the sub==0 init block, and the
//    D_00810845/D_00810766/D_0028A518/D_0028A51C/D_0024FD50/D_00275638/
//    D_00275648 externs all had to be declared as incomplete arrays (`T[]`)
//    rather than scalars to get the correct mix of %gp_rel vs lui/%lo forms
//    at -sdatathreshold 4 (array-typed externs are never gp_rel-eligible;
//    scalar externs are, if their size is <= threshold).
//
// SEMANTICS: Actor state-machine init/update (companion to func_001BFFD0,
// which self+0x24 is wired to on success). State byte at self+4:
//   0: debug-skip check (D_00810845 & 0x20) -> if set, force debug-fail
//      color D_00810766=0xFF, install the "failed spawn" update vtable
//      func_001BF6B0, notify via func_001B6660(&D_829110, 2), and bail.
//      Otherwise builds a scratch 4x4-ish transform via the VU scratch
//      block D_700038A0.. (col0 (0,1,0), col1 identity-ish, w=1) and
//      allocates a companion actor via func_001AFA90(2,2). On success,
//      copies self's pos(+0xB0) and rot(+0xC0) into the new actor, tags it
//      class/type bytes +3=0x12, +0xD=2, installs its update vtable
//      func_001BFFD0, cross-links self<->companion (+0x20 / +0x24), inits
//      self's bone/segment array (func_001D0C80) and animation slot
//      (func_001D0D40) with type D_0028A518, clears the tail block +0x1F0
//      short at +2 and self+0x1F0 short at 0 (bone_init_default_1 does the
//      pose reset), seeds self+0x58 with D_0028A51C, advances state to 1,
//      timer self+0x34=0x50, installs default bone-matrix tables at +0x30
//      and tail+0x18, rebuilds the world matrix (func_001C6380), then
//      computes a derived look-basis: transforms the pos by a translate
//      matrix, cross/normalizes into tail+0xC/0x10/0x14 (an up-like unit
//      vector) and tail+8 (a dot-derived scalar, e.g. cos of an angle)
//      using the VU helpers (func_001026A0 matrix*vec, func_001028D0
//      vsub, func_001028B8 vadd, func_00102760 normalize, func_00102738
//      dot). On alloc failure, sets self+4=3 (teardown next tick).
//   1: sub-state self+5 drives a one-shot init (clear self+0x28, bump
//      self+5) then every tick calls func_001BF630(&D_008102B0, self,
//      tail) (proximity/visibility test against the shared player actor);
//      on success (and if the one-shot debug latch at 0x70003B64 is
//      clear) plays a proximity cue (func_001FBD50(self, 0x444, 0, 300.0))
//      and sets self+0x28=1, else self+0x28=0. If the "hit" flag self+0x36
//      is set, forces teardown (self+0=3, self+0x36=0). Rebuilds the world
//      matrix (func_001C6380); if self+0x28 is set, ramps the blend weight
//      (func_001D0D60(self+0x90, 1.0) into tail+0). Runs the common per-
//      object tick (func_001B17A0) then the object's virtual update
//      function pointer at self+0x4C.
//   2: advances straight to state 3 (no-op tick).
//   3: releases the bone-array handle (func_001AF890) and frees the actor
//      back to the pool (func_001AFC10).
extern void bone_init_default_1(unsigned char *self);
extern void func_001026A0(void *out, void *mtx, void *vec);
extern float func_00102738(void *a, void *b);
extern void func_00102760(void *out, void *in);
extern void func_001028B8(void *out, void *a, void *b);
extern void func_001028D0(void *out, void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern void func_001AF890(unsigned char *handle);
extern unsigned char *func_001AFA90(int cls, int sub);
extern void func_001AFC10(unsigned char *handle);
extern void func_001B17A0(unsigned char *self);
extern void func_001B6660(void *list, int a1, unsigned char cause);
extern int func_001BF630(void *player, unsigned char *self, unsigned char *tail);
extern void func_001C6380(unsigned char *self);
extern void func_001D0C80(unsigned char *self, int type);
extern void func_001D0D40(unsigned char *self, void *tbl, int type, int flag);
extern short func_001D0D60(int slot, float weight);
extern void func_001FBD50(unsigned char *self, int sound, int a2, float radius);
extern unsigned char D_008102B0[];
extern unsigned char D_00810766[];
extern unsigned char D_00810845[];
extern unsigned char D_829110[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038E0[];
extern char D_700038F0[];
extern int D_0028A518[];
extern int D_0028A51C[];
extern void *D_0024FD50[];
extern void *D_00275638;
extern void *D_00275648;
extern void func_001BF6B0(void);
extern void func_001BFFD0(void);

void func_001C02E0(unsigned char *arg0) {
    unsigned char *self;
    unsigned char *comp;
    unsigned char *tail;
    unsigned char *player;
    unsigned char st;
    unsigned char sub;

    self = arg0;
    player = D_008102B0;
    st = *(unsigned char *)(self + 4);
    tail = self + 0x1F0;
    switch (st) {
    case 0:
        if (D_00810845[0] & 0x20) {
            D_00810766[0] = 0xFF;
            *(void **)(self + 0x10) = (void *)func_001BF6B0;
            func_001B6660(D_829110, 2, st);
            return;
        }
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = 2.0f;
        *(float *)0x700038A8 = 5.0f;
        *(float *)0x700038AC = 1.0f;
        comp = func_001AFA90(2, 2);
        if (comp != 0) {
            func_00102948(comp + 0xB0, self + 0xB0);
            func_00102948(comp + 0xC0, self + 0xC0);
            *(unsigned char *)(comp + 3) = 0x12;
            *(unsigned char *)(comp + 0xD) = 2;
            *(void **)(comp + 0x10) = (void *)func_001BFFD0;
            *(unsigned char **)(comp + 0x20) = self;
            *(unsigned char **)(self + 0x24) = comp;
            func_001D0C80(self, D_0028A518[0]);
            func_001D0D40(self, D_0024FD50, 0x5B, 1);
            *(short *)(tail + 2) = 0;
            *(short *)(tail + 0) = 0;
            bone_init_default_1(self);
            *(int *)(self + 0x58) = D_0028A51C[0];
            *(unsigned char *)(self + 0) = 3;
            *(unsigned char *)(self + 4) = 1;
            *(short *)(self + 0x34) = 0x50;
            *(void **)(self + 0x30) = &D_00275638;
            *(void **)(tail + 0x18) = &D_00275648;
            func_001C6380(self);
            func_00102948(D_700038A0, self + 0xB0);
            *(float *)0x700038B0 = 0.0f;
            *(float *)0x700038AC = 1.0f;
            *(int *)0x700038B4 = 0x41F00000;
            *(float *)0x700038B8 = 0.0f;
            *(float *)0x700038BC = 1.0f;
            func_001026A0(D_700038B0, self + 0xD0, D_700038B0);
            *(float *)0x700038BC = 1.0f;
            func_001028D0(D_700038E0, D_700038B0, D_700038A0);
            *(float *)(tail + 0xC) = *(float *)0x700038E0;
            *(float *)(tail + 0x10) = *(float *)0x700038E4;
            *(float *)(tail + 0x14) = *(float *)0x700038E8;
            func_00102760(D_700038E0, D_700038E0);
            *(float *)0x700038B0 = 0.0f;
            *(int *)0x700038B4 = 0xC1F00000;
            *(float *)0x700038B8 = 0.0f;
            *(float *)0x700038BC = 1.0f;
            func_001028B8(D_700038B0, D_700038A0, D_700038B0);
            *(float *)0x700038BC = 1.0f;
            func_001028D0(D_700038F0, D_700038B0, D_700038A0);
            func_00102760(D_700038F0, D_700038F0);
            *(float *)(tail + 8) = func_00102738(D_700038E0, D_700038F0);
            return;
        }
        *(unsigned char *)(self + 4) = 3;
        return;
    case 1:
        sub = *(unsigned char *)(self + 5);
        switch (sub) {
        case 0:
            *(short *)(self + 0x28) = 0;
            *(unsigned char *)(self + 5) = *(unsigned char *)(self + 5) + 1;
            /* fallthrough */
        case 1:
            if (func_001BF630(player, self, tail) != 0) {
                if (*(unsigned char *)0x70003B64 == 0) {
                    func_001FBD50(self, 0x444, 0, 300.0f);
                }
                *(short *)(self + 0x28) = 1;
            } else {
                *(short *)(self + 0x28) = 0;
            }
            break;
        }
        if (*(short *)(self + 0x36) != 0) {
            *(unsigned char *)(self + 0) = 3;
            *(short *)(self + 0x36) = 0;
        }
        func_001C6380(self);
        if (*(short *)(self + 0x28) != 0) {
            *(short *)(tail + 0) = func_001D0D60(*(int *)(self + 0x90), 1.0f);
        }
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        return;
    case 2:
        *(unsigned char *)(self + 4) = 3;
        return;
    case 3:
        func_001AF890(*(unsigned char **)(self + 0x90));
        func_001AFC10(self);
        return;
    }
}
