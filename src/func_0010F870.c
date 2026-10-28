// COMPILER: eegcc
// CFLAGS: -O2 -fno-schedule-insns
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 73.56% NEARMISS).
// Fixes read from the original instructions:
// - The "D_FFFFF relocation as counter" wall was a splat symbolization artifact (the
//   normalized target has a plain 0x100000 constant); the delay loop is an ordinary
//   counter decremented from 0x100000 until it reaches -1. The four padding slots in
//   its body are ee-gcc's own short-loop padding, not source statements.
// - The bind block address is reloaded at the top of each poll (for(;;) with the
//   pointer assigned inside the loop), as in the sibling func_0010FAD0.
// - The original object was built without the first scheduling pass
//   (-fno-schedule-insns): that is what places the latch store before the zero return
//   value (with the store's high half in the return register).
//
// SIF RPC bind retry loop (libsdk client init): repeatedly binds the request block
// D_00279240 to RPC server 0x80000003 via func_0010E6F8(block, 0x80000003, 0); a
// negative result returns -1. Once the block's +0x24 server word becomes nonzero, the
// latch D_00241C90 is cleared (marks the client ready) and 0 is returned. Between
// polls it burns a hand-rolled busy-wait delay loop.

extern int func_0010E6F8(void *p, unsigned int flags, int x);
extern unsigned char D_00279240[];
extern int D_00241C90;

int func_0010F870(void)
{
    void *s0;

    for (;;) {
        s0 = (void *)D_00279240;
        if (func_0010E6F8(s0, 0x80000003u, 0) < 0)
            return -1;
        if (*(int *)((char *)s0 + 0x24) != 0)
            break;
        {
            int i = 0x00100000;
            do {
                i--;
            } while (i != -1);
        }
    }
    D_00241C90 = 0;
    return 0;
}
