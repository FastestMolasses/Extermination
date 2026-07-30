// NEARMISS func_00112758  (vram 0x00112758, 0x290 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.60% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall — exactly one adjacent-op swap (sw D_00241D48 vs lui %hi(D_00241D08)) in the RPC-failure block; 162/164 words reloc-resolved byte-identical. Target ordering needs the store to be a scheduling dependence of the following load; no C-level form reproduces it (volatile, same...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// SIF-RPC wrapper (same family as the already-committed func_00113F68): check the RPC client is
// bound, publish the send buffer pointer, write back the D-cache for both buffers, raise the
// busy flag D_00241D48 to 3, do the blocking RPC (function #4), then copy the reply out of the
// UNCACHED (| 0x20000000) mirror of the receive buffer into the caller's buffer.  The reply
// header word at D_00279B84 selects the long (0x810) or short (0x400) payload; the word at
// D_00279B80 is the value returned to the caller.  CreateSema() here is the splat name for the
// semaphore/signal call that releases the RPC lock.
//

extern int func_00111F18(int);
extern void func_0010DFD8(void *, int);
extern int func_0010E8A8(void *, int, int, void *, int, void *, int, void *, void *);
extern int CreateSema(int);
extern void *memcpy(void *, const void *, unsigned int);

extern int D_00241D08;
extern int D_00241D48;
extern unsigned char D_00279B80[];
extern unsigned char D_00279E00[];
extern void *D_0027A2D0;
extern unsigned char D_0027A300[];

int func_00112758(void *dst)
{
    int ret;

    if (func_00111F18(8) == 0)
        return 0;

    D_0027A2D0 = D_0027A300;
    func_0010DFD8(D_0027A300, 0x810);
    func_0010DFD8(&D_0027A2D0, 0xC);

    D_00241D48 = 3;
    if (func_0010E8A8(D_00279E00, 4, 0, &D_0027A2D0, 0xC, D_00279B80, 8, 0, 0) < 0) {
        D_00241D48 = 0;
        CreateSema(D_00241D08);
        return 0;
    }
    D_00241D48 = 0;

    ret = *(int *)(D_00279B80 + 0);
    if (*(int *)(D_00279B80 + 4) == 0)
        memcpy(dst, (void *)((unsigned int)D_0027A300 | 0x20000000), 0x400);
    else
        memcpy(dst, (void *)((unsigned int)D_0027A300 | 0x20000000), 0x810);

    CreateSema(D_00241D08);
    return ret;
}
