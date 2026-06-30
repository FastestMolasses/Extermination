// NEARMISS func_00182BF0  (vram 0x00182BF0, 0x148 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 69.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// CW branch-inversion/dead-b idiom dominates: each of the ~7 `return 1` exits is lowered by the target as a forward `bnez/bc1t` with the v0=1 set in the delay slot and a `b` into a split epilogue that does `lq ra` in its delay slot; mwcc inverts every guard, nops the delay slot, and unifies all exi...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Gate/eligibility check on an entity (arg0). Series of early `return 1` bail-
// outs: if D_008106BC set then bail when D_0081083C set else clear D_008106BC;
// bail if float[arg0+0x220] <= 0; bail if byte[arg0+0xF]==0x63; if D_0081083C
// set then set D_008106BC=1 and bail; bail if D_008106F1 set; bail if
// func_0021BB00(arg0) returns nonzero; read byte[arg0+0x1F0] and bail if it is
// 0x3C, 0x3D, or in [0xB,0xE) (the v-0xB < 3 test). Finally, if either float at
// arg0+0x22C or arg0+0x224 is nonzero, zero both and set byte[arg0]=1; return 0.
//
// Logic fully recovered (the trailing float-clear body at the bottom matches
// instruction-for-instruction). DOMINANT WALL: the CW branch-inversion /
// dead-b idiom -- each `return 1` is emitted by the target as
// `<test> bnez/bc1t forward; addiu v0,1 (delay); b <split-epilogue>; lq ra(delay)`
// jumping into a SECOND epilogue entry that does `lq ra` before the final
// `lq s0; jr ra`. mwcc instead inverts every guard (`beqz/bc1f`), inserts a nop
// delay slot, and routes all exits through one unified `b <end>` epilogue.
// Secondary: FP-compare register coloring (target `c.eq.s f0,f1` with field in
// f0/zero in f1; mwcc swaps to field in f1/zero in f0). Both are documented
// CW-vs-mwcc lowering walls that 2.3.3 does not fix. Tried single-exit, OR-
// chaining the 0x3C/0x3D/<3 triple (best, +8%), separate ifs (worse). Parked.
extern int func_0021BB00(char *p);
extern unsigned char D_008106BC;
extern unsigned char D_008106F1;
extern unsigned char D_0081083C;

int func_00182BF0(char *arg0) {
    unsigned char v;

    if (D_008106BC != 0) {
        if (D_0081083C != 0) {
            return 1;
        }
        D_008106BC = 0;
    }
    if (*(float *)(arg0 + 0x220) <= 0.0f) {
        return 1;
    }
    if (*(unsigned char *)(arg0 + 0xF) == 0x63) {
        return 1;
    }
    if (D_0081083C != 0) {
        D_008106BC = 1;
        return 1;
    }
    if (D_008106F1 != 0) {
        return 1;
    }
    if (func_0021BB00(arg0) != 0) {
        return 1;
    }
    v = *(unsigned char *)(arg0 + 0x1F0);
    if ((v == 0x3C) || (v == 0x3D) || ((unsigned int)(v - 0xB) < 3)) {
        return 1;
    }
    if ((*(float *)(arg0 + 0x22C) != 0.0f) || (*(float *)(arg0 + 0x224) != 0.0f)) {
        *(float *)(arg0 + 0x22C) = 0.0f;
        *(float *)(arg0 + 0x224) = 0.0f;
        *(char *)(arg0 + 0) = 1;
    }
    return 0;
}
