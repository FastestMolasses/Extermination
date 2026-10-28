// COMPILER: eegcc
// CFLAGS: -O2

/* Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 0.00% NEARMISS).
 * Fixes read from the original instructions:
 * - the compiler was wrong: this is ee-gcc SDK code (the MPEG/IPU decoder), not
 *   mwcc; ee-gcc -O2 turns a void call in tail position into the 3-instruction
 *   tail jump the original has, so no asm wrapper is needed.
 * - func_0010A3A8 takes an int (its definition in src/), not an int pointer.
 *
 * MPEG decoder error path (named after its "Unknown Extension..."
 * message): passes the address D_0026B728 to func_0010A3A8, which queues it on
 * the live handler chain or dispatches it directly. */

extern int D_0026B728[];
extern void func_0010A3A8(int a0);

void sub_Unknown_Extension(void)
{
    func_0010A3A8((int)D_0026B728);
}
