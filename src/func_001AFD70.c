// NEARMISS func_001AFD70  (vram 0x001AFD70, 0xEC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.46% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Saved-register-allocation-ORDER wall (identical residual under 991202 and 2.3.3). Entire body matches: mode dispatch, class-1 skip paths, func_001CB590 call, behavior-pointer dispatch, scratchpad walk counter, link refresh. Sole residual is the cur/next saved-register SWAP -- CW colors cur=s1/nex...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Actor active-list TICK (the per-frame "world services" walker, called
// from the gameplay frame func_001AE5E0 after the player update). Walks the
// active doubly-linked list (head D_00275BC0, next link at +0x1C):
// mode 1 skips class-1 actors, mode 2 ticks ONLY class-1 actors, any other
// mode ticks all. Per ticked actor: func_001CB590(actor, 0x2F0,
// bonecount@+0x09) publishes the "current actor" global (D_00275B44) and
// sizes the shared bone work array; then the per-frame flag byte +0x01 is
// cleared and the BEHAVIOR POINTER *(+0x10) is invoked. Afterwards the
// canonical actor's +0x18/+0x1C links are refreshed from the walked node.
// A per-frame walk counter lives in scratchpad at 0x70003B8A.
//
// NEARMISS 97.46% (mwcc 991202 AND 2.3.3 identical): the entire body matches
// (mode dispatch, skip paths, func_001CB590 call, behavior-pointer dispatch,
// link refresh, scratchpad counter). Sole residual is the cur/next
// saved-register SWAP -- CW colors cur=s1/next=s0 (next=s0 by declaration
// order); mwcc allocates by web start and cur's web (head load + loop guard +
// phi) necessarily starts first, so mwcc gives cur s0. Seeding `cur=next`
// before the loop is copy-propagated away. This is the documented
// saved-register-allocation-ORDER wall (late-init case), plus the one
// matching delay-slot fill at the loop bottom (CW `bnez s1; nop` vs mwcc
// hoisting the next iteration's `lui at,0x7000` into the slot). 2.3.3 does
// NOT crack this class -- permuter territory.
extern unsigned char *D_00275BC0;
extern unsigned char *D_00275B44;
extern void func_001CB590(unsigned char *actor, int size, int nbones);

void func_001AFD70(int mode) {
    int m;
    unsigned char *cur;
    unsigned char *next;
    unsigned char *pub;
    void (*behavior)(unsigned char *);

    m = mode;
    cur = D_00275BC0;
    *(volatile short *)0x70003B8A = 0;
    while (cur != 0) {
        next = *(unsigned char **)(cur + 0x1C);
        *(volatile short *)0x70003B8A += 1;
        if (m == 1) {
            if ((cur[0x2] & ~0xE0) == 1) { cur = next; continue; }
        } else if (m == 2) {
            if ((cur[0x2] & ~0xE0) != 1) { cur = next; continue; }
        }
        func_001CB590(cur, 0x2F0, cur[0x9]);
        pub = D_00275B44;
        pub[0x1] = 0;
        behavior = *(void (**)(unsigned char *))(pub + 0x10);
        behavior(pub);
        *(int *)(pub + 0x18) = *(int *)(cur + 0x18);
        *(int *)(pub + 0x1C) = *(int *)(cur + 0x1C);
        cur = next;
    }
}
