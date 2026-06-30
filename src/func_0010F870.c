// NEARMISS func_0010F870  (vram 0x0010F870, 0x88 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 73.56% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Busy-wait delay loop in target is emitted as lui %hi(D_FFFFF); addiu %lo(D_FFFFF) using linker symbol D_FFFFF(=0xFFFFF) reloc as the per-iteration -1 decrement (SDK delay idiom); clean C cannot reproduce this reloc-as-counter. Also target keeps %hi in callee-saved s1 (frame 0x30, 3 saves) and rematerializes s0 each ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_0010E6F8(void *a0, unsigned int a1, int a2);
extern unsigned char D_00279240[];
extern int D_00241C90;

int func_0010F870(void)
{
    void *s0 = (void *)D_00279240;
    int r;

    for (;;) {
        r = func_0010E6F8(s0, 0x80000003, 0);
        if (r < 0)
            return -1;
        if (*(int *)((char *)s0 + 0x24) != 0)
            break;
        {
            int i;
            for (i = 0xFFFFF; i != -1; i--)
                ;
        }
    }

    D_00241C90 = 0;
    return 0;
}
