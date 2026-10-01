// NEARMISS func_0010E3A8  (vram 0x0010E3A8, 0xB4 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 43.91% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc layout of the command switch and the client-record reloads (not iterated).
//
// The function links from the asm body in src/func_0010E3A8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK (SIF RPC client): completion callback of an RPC packet. pkt+0x1C is
// the client record. Command 0x80000009 (bind reply) copies the server
// handle and buffers (+0x24 / +0x28 / +0x2C) into the client (+0x24 / +0x14
// / +0x18); command 0x8000000A (call reply) runs the client's end function
// (+0x1C) with its argument (+0x20). Then the client's semaphore (+8) is
// signalled from the interrupt context (iSignalSema) when valid, its packet
// (+0) is freed with func_0010E318 and cleared.
extern int iSignalSema(int sema);
extern void func_0010E318(int pkt);

void func_0010E3A8(char *pkt) {
    char *cd;
    void (*end)(int);

    switch (*(unsigned int *)(pkt + 0x20)) {
    case 0x80000009:
        cd = *(char **)(pkt + 0x1C);
        *(int *)(cd + 0x24) = *(int *)(pkt + 0x24);
        *(int *)(cd + 0x14) = *(int *)(pkt + 0x28);
        *(int *)(cd + 0x18) = *(int *)(pkt + 0x2C);
        break;
    case 0x8000000A:
        cd = *(char **)(pkt + 0x1C);
        end = *(void (**)(int))(cd + 0x1C);
        if (end != 0) {
            end(*(int *)(cd + 0x20));
        }
        break;
    }
    cd = *(char **)(pkt + 0x1C);
    if (*(int *)(cd + 8) >= 0) {
        iSignalSema(*(int *)(cd + 8));
    }
    func_0010E318(*(int *)(*(char **)(pkt + 0x1C)));
    *(int *)(*(char **)(pkt + 0x1C)) = 0;
}
