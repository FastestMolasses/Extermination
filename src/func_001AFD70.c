// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at 0x70003B8A
// are referenced as relocated externs (D_7000xxxx), as the original build did
// (see tools/match/spad_symbolize.py). A literal address let mwcc CSE and
// speculate `lui at,0x7000` into delay slots that the target leaves as nop.
// objdiff is 100.0% once build.py _SPAD_SYMS lists these addresses (so the
// expected object carries the same relocations); the linked bytes are identical.

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
extern volatile short D_70003B8A[16];
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
    D_70003B8A[0] = 0;
    while (cur != 0) {
        next = *(unsigned char **)(cur + 0x1C);
        D_70003B8A[0] += 1;
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
