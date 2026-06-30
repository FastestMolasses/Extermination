// NEARMISS func_00203E60  (vram 0x00203E60, 0xE0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/control-flow/param-mapping (leading unused arg, real params $a1/$a2)/addressing (0x50000-lui + lw 0x8 split)/func_002063B0 stack-slot layout (a2->sp+0x58, a3->sp+0x54)/all eight func_00203F40 call args byte-identical. Sole residual is a register-coloring permutation of the four callee-saved ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// objdiff 91.0% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0);
// pinned 991202 = 77.25%. Body, control flow, parameter mapping (leading unused
// arg, real params in $a1/$a2), addressing (the 0x50000-lui + lw 0x8 split for
// *(arg2+0x50008), recovered via ((int*)(arg2+0x50000))[2]), the func_002063B0
// stack-slot layout (a2->sp+0x58, a3->sp+0x54), and ALL eight func_00203F40 call
// arguments are byte-identical.
//
// Sole residual: a register-coloring permutation of the four callee-saved
// values. The target colors src->$s1, dst_cap (*(arg1+0xC)-4)->$s2, region
// base (arg2)->$s0, clamped len->$s3 and keeps dst_cap live across the
// func_002063B0 call to recompute the trailing (cap-len) arg from $s2; mwcc
// picks a different s-reg assignment (region->$s2, src->$s0, len->$s1, arg1->$s3)
// and recomputes *(arg1+0xC)-4 after the call. Equivalent code, permuted
// coloring -> permuter. (NOT the clean-store delay nop.)
//
// Sets up a windowed block copy. The source cursor (arg1+8, +4) is wrapped
// into the region [arg2 .. arg2 + *(arg2+0x50008)); the run length is clamped
// to the destination capacity (arg1+0xC, -4). Queries the active DMA/region
// descriptor via func_002063B0, performs the copy through func_00203F40,
// releases the region via func_00206470, and returns 1 if any bytes moved.
int func_00203F40();
void func_002063B0(char *a0, int *a1, int *a2, int *a3, int *t0);
void func_00206470(char *a0, int n);
extern char D_002DF800[];

int func_00203E60(int unused, char *arg1, char *arg2) {
    int sp50, sp58, sp54, sp5C;
    unsigned int sz;
    unsigned int src;
    unsigned int end;
    int n;

    sz = ((int *)(arg2 + 0x50000))[2];
    src = *(int *)(arg1 + 8) + 4;
    end = (unsigned int)arg2 + sz;
    if (src >= end) {
        src -= sz;
    }
    n = end - src;
    if ((unsigned int)(*(int *)(arg1 + 0xC) - 4) < (unsigned int)(end - src)) {
        n = *(int *)(arg1 + 0xC) - 4;
    }
    func_002063B0(D_002DF800, &sp50, &sp54, &sp58, &sp5C);
    n = func_00203F40(sp50, sp54, sp58, sp5C, src, n,
                      (int)arg2, (*(int *)(arg1 + 0xC) - 4) - n);
    func_00206470(D_002DF800, n);
    if (n > 0) {
        return 1;
    }
    return 0;
}
