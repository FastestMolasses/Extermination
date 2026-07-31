// COMPILER: eegcc
// CFLAGS: -O2
//
// MPEG macroblock skip handler. Marks the active slot (D_002418F8 is 2 x 0x140-byte slots
// with the active-slot index at +0x280) as "skipped", zeroes the caller's motion-vector
// block in the 2-mode, sets the prediction type from D_002414AC, and clears bit 0 of the
// flag word. Returns 0 (with the "skipped macroblock in I picture" diagnostic) when the
// picture type D_0024147C is 1, else 1.
//
// ee-gcc note: a run of INDEPENDENT adjacent stores is emitted in REVERSE source order —
// hence a0[5],a0[4],a0[1],a0[0] to obtain sw 0/0x14/0x10/0x4 and a2[1],a2[0] to obtain
// sw 0(a2) before sw 4(a2). The `fld` temp is required: writing (D_002414AC == 2) twice
// makes gcc reload the global, since the store through a2 may alias it.

extern char D_002418F8[];     /* 2 x 0x140-byte slots, then the active-slot index at +0x280 */
extern int D_0024147C;
extern int D_002414AC;
extern int D_0024153C;
extern char D_0026B460[];

extern void func_0010A3A8(char *);

int sub_skiped_macroblock_in_I_picure_is(int *a0, int *a1, int *a2, int *a3)
{
    char *base;
    char *slot;
    int ok;
    int fld;

    ok = 1;
    base = D_002418F8;
    slot = base + *(int *)(base + 0x280) * 0x140;
    D_0024153C = 1;
    *(int *)(slot + 0x13C) = 1;

    if (D_0024147C == 2) {
        a0[5] = 0;
        a0[4] = 0;
        a0[1] = 0;
        a0[0] = 0;
    }

    if (D_002414AC == 3) {
        *a1 = 2;
    } else {
        *a1 = 1;
        fld = (D_002414AC == 2);
        a2[1] = fld;
        a2[0] = fld;
    }

    if (D_0024147C == 1) {
        func_0010A3A8(D_0026B460);
        ok = 0;
    }

    *a3 = *a3 & ~1;
    return ok;
}
