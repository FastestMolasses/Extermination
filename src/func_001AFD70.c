// INCLUDE_ASM func_001AFD70  (vram 0x001AFD70, 236 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001AFD70.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// BEST READABLE-C ATTEMPT (2026-06-10): 97.46%, wall-blocked. The whole
// walk body matches (mode dispatch, skip paths, func_001CB590 call,
// behavior-pointer dispatch, link refresh). TWO STEERINGS that mattered:
// (a) read `next = cur->next` BEFORE the scratchpad counter increment -
// CW's lw lands mid-counter-sequence and the counter sh in the mode
// bne's delay slot, and mwcc reproduces both from that source order;
// (b) idiom 12c on the func_001CB590 call - materialize `sz = 0x2F0;`
// as its own statement so a1 is set before the jal and the actor
// pointer paddub lands in the jal delay slot. Residual: the cur/next
// saved-register swap (CW: next=s0/cur=s1 by declaration order; mwcc
// allocates by web start and cur's web - head load + loop guard + phi -
// necessarily starts first, so mwcc gives cur s0; `cur = next` seeding
// before the loop gets copy-propagated away; this is the documented
// saved-register-allocation-ORDER wall, late-init case), plus one
// delay-slot fill #13 (CW `bnez s1; nop` at loop bottom vs mwcc
// hoisting the next iteration's `lui at,0x7000` into the slot).
//
// Attempt source (compiles, logic verified against the .s):
//
// // Actor active-list TICK (the per-frame "world services" walker, called
// // from the gameplay frame func_001AE5E0 after the player update). Walks
// // the active doubly-linked list (head D_00275BC0, next +0x1C):
// // mode 1 skips class-1 actors, mode 2 ticks ONLY class-1 actors, any
// // other mode ticks all. Per ticked actor: func_001CB590(actor, 0x2F0,
// // bonecount@+0x09) publishes the "current actor" globals (D_00275B44/
// // B48) and sizes the shared bone work array; then the per-frame visible
// // flag byte +0x01 is cleared and the BEHAVIOR POINTER *(+0x10) is
// // invoked. Afterwards the (possibly relocated) canonical actor's
// // prev/next links are refreshed from the walked node. A per-frame walk
// // counter lives in scratchpad at 0x70003B8A.
// extern unsigned char *D_00275BC0;  // active-list head
// extern unsigned char *D_00275B44;  // current-actor global
// extern void func_001CB590(unsigned char *actor, int size, int nbones);
//
// void func_001AFD70(int mode) {
//     int m;
//     unsigned char *cur;
//     unsigned char *next;
//     unsigned char *pub;
//     void (*behavior)(unsigned char *);
//     int nb;
//     int sz;
//
//     m = mode;
//     next = D_00275BC0;
//     *(volatile short *)0x70003B8A = 0;
//     cur = next;
//     while (cur != 0) {
//         next = *(unsigned char **)(cur + 0x1C);
//         *(volatile short *)0x70003B8A += 1;
//         if (m == 1) {
//             if ((cur[0x2] & ~0xE0) == 1) {
//                 cur = next;
//                 goto bottom;
//             }
//         } else if (m == 2) {
//             if ((cur[0x2] & ~0xE0) != 1) {
//                 cur = next;
//                 goto bottom;
//             }
//         }
//         nb = cur[0x9];
//         sz = 0x2F0;
//         func_001CB590(cur, sz, nb);
//         pub = D_00275B44;
//         pub[0x1] = 0;
//         behavior = *(void (**)(unsigned char *))(pub + 0x10);
//         behavior(pub);
//         *(int *)(pub + 0x18) = *(int *)(cur + 0x18);
//         *(int *)(pub + 0x1C) = *(int *)(cur + 0x1C);
//         cur = next;
// bottom:;
//     }
// }
