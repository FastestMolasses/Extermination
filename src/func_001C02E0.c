// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x700038A0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.51% NEARMISS).
// Fixes read from the original instructions:
//  - func_001AFA90 and func_001B6660 take ONE argument (their definitions in
//    src/): the "extra" a1/a2 values were just leftovers from the state
//    dispatch, not arguments.
//  - The switch cases end in `break`, not `return` (one shared epilogue).
//  - The allocation is `if (comp != 0) { copy/link } else { self+4 = 3; break; }`
//    followed by the rest of the init: the "dead duplicate" fail arm was the
//    delay-slot fill copying the else block's first instruction, not a
//    compiler wall.
//  - The scratchpad word 0x700038A0 is a relocated extern (idiom-32, opted in
//    with // SPAD:), declared `float D_700038A0[]` (array: never gp_rel) and
//    passed uncast to the vector helpers; as a symbol its upper half is no
//    longer speculated into the debug-check branch slot. 0x700038A4..AC stay
//    literals.
//
// SEMANTICS: Actor state-machine init/update (companion to func_001BFFD0,
// which self+0x24 is wired to on success). State byte at self+4:
//   0: debug-skip check (D_00810845 & 0x20) -> if set, force debug-fail
//      color D_00810766=0xFF, install the "failed spawn" update vtable
//      func_001BF6B0, notify via func_001B6660(&D_829110), and bail.
//      Otherwise builds a scratch 4x4-ish transform via the VU scratch
//      block D_700038A0.. (col0 (0,1,0), col1 identity-ish, w=1) and
//      allocates a companion actor via func_001AFA90(2). On success,
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
extern unsigned char *func_001AFA90(int cls);
extern void func_001AFC10(unsigned char *handle);
extern void func_001B17A0(unsigned char *self);
extern void func_001B6660(void *list);
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
extern float D_700038A0[];
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
            func_001B6660(D_829110);
            break;
        }
        D_700038A0[0] = 0.0f;
        *(float *)0x700038A4 = 2.0f;
        *(float *)0x700038A8 = 5.0f;
        *(float *)0x700038AC = 1.0f;
        comp = func_001AFA90(2);
        if (comp != 0) {
            func_00102948(comp + 0xB0, self + 0xB0);
            func_00102948(comp + 0xC0, self + 0xC0);
            *(unsigned char *)(comp + 3) = 0x12;
            *(unsigned char *)(comp + 0xD) = 2;
            *(void **)(comp + 0x10) = (void *)func_001BFFD0;
            *(unsigned char **)(comp + 0x20) = self;
            *(unsigned char **)(self + 0x24) = comp;
        } else {
            *(unsigned char *)(self + 4) = 3;
            break;
        }
        {
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
        }
        break;
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
        break;
    case 2:
        *(unsigned char *)(self + 4) = 3;
        break;
    case 3:
        func_001AF890(*(unsigned char **)(self + 0x90));
        func_001AFC10(self);
        break;
    }
}
