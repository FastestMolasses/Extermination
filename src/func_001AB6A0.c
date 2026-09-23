// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// MATCH NOTE (m2-matching lane): the state test is a `switch` whose case
// labels are written 4, 1, 2. mwcc lowers a sparse switch to a compare chain in
// REVERSE label order (2, 1, 4 here, as the target tests), falls to a `b` past
// the body with a nop slot when no case matches, and keeps the case bodies in
// label order, so promote (states 1/4 -> 2) falls through into run. The earlier
// goto chain reached 90.38%.
//
// Semantics: the task-slot scheduler. Walks the 0x20-byte slots from
// D_0028A750 up to D_0028A7B0, keeping the cursor in the scratchpad pointer
// 0x70003B6C (re-read after every step, as the target does). A slot whose state
// byte is 1 or 4 is promoted to 2; a slot in state 2 (including a just-promoted
// one) has its function pointer at +4 called. The port's ORIGINAL_FRAME_ORDER.md
// traces the world frame running from slot 0 here.

extern char * volatile D_70003B6C;                  /* PS2 scratchpad @ 0x70003B6C */

extern char D_0028A750[];
extern char D_0028A7B0[];

void func_001AB6A0(void) {
    char *slot;
    unsigned char state;
    D_70003B6C = D_0028A750;
    do {
        slot = D_70003B6C;
        state = *(unsigned char *)slot;
        switch (state) {
        case 4:
        case 1:
            *(unsigned char *)slot = 2;
        case 2:
            slot = D_70003B6C;
            (*(void (**)(void))(slot + 4))();
        }
        slot = D_70003B6C;
        slot += 0x20;
        D_70003B6C = slot;
    } while (slot < D_0028A7B0);
}
