// NEARMISS func_001D7A80  (vram 0x001D7A80, 0xAC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 57.42% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation (permuter territory). Body fully recovered and structurally correct: every store (sb +3=0x10, sw +4=0, sh +0=7, advance e+0x10 by 0x80, sq +0x10=0, sw +0x1C=0x50000006, sd +0x20=GIF-tag, sd +0x28=0x43431) and the five quadword copies (+0x30=*arg4, +0x40=*arg3...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (best 57.4% mwcc233, 30.9% mwcc991202) -- logic fully recovered;
// residual is register-allocation order only (permuter territory).
//
// Builds a GIF/DMA packet into a ring entry. Indexes the global ring base
// D_00275670 by arg0 (stride 4) to reach the entry e; the active write cursor
// lives in the pointer field e+0x10. Through that cursor p it writes a packet
// header: byte +3 = 0x10, word +4 = 0, halfword +0 = 7; then advances the
// cursor by 0x80. Into the just-claimed 0x80 block it stores: a zeroed quadword
// at +0x10, the GIF-tag-ish word 0x50000006 at +0x1C, a 64-bit tag at +0x20
// formed from the sign-extended low 32 bits of (arg1 | 0x116) shifted left 47
// OR'd with 0x50004000_00008001, the 64-bit 0x43431 at +0x28, then five source
// quadwords: *arg4 -> +0x30, *arg3 -> +0x40, *arg2 -> +0x50, arg3[0x10] -> +0x60,
// arg2[0x10] -> +0x70. Returns the packet body pointer p + 0x10. The active
// cursor is reloaded from e+0x10 before each early field write (matching the
// target's repeated `lw t2,0x10(t4)`). Sibling func_001D7080 (parked asm-void)
// confirms the same packet-builder shape.
//
// Wall: mwcc 2.3.3 colors the pointer/temp regs $a5/$a6/$a7 where CW 2.3.1 used
// $t1..$t4, and that re-schedules the constant block -- a register-allocation
// permutation, not the clean-store nop wall. Park for the permuter pass.
typedef unsigned __int128 uint128;
extern char *D_00275670;

char *func_001D7A80(int arg0, int arg1, uint128 *arg2, uint128 *arg3, uint128 *arg4) {
    char *e;
    char *p;

    e = D_00275670 + (arg0 * 4);
    p = *(char **)(e + 0x10);
    *(char *)(p + 3) = 0x10;
    p = *(char **)(e + 0x10);
    *(int *)(p + 4) = 0;
    p = *(char **)(e + 0x10);
    *(short *)(p + 0) = 7;
    p = *(char **)(e + 0x10);
    *(char **)(e + 0x10) = p + 0x80;
    *(uint128 *)(p + 0x10) = 0;
    *(int *)(p + 0x1C) = 0x50000006;
    *(long long *)(p + 0x20) =
        ((long long)(int)(arg1 | 0x116) << 0x2F) | (((long long)0x50004000 << 0x20) | 0x8001);
    *(long long *)(p + 0x28) = 0x43431;
    *(uint128 *)(p + 0x30) = *arg4;
    *(uint128 *)(p + 0x40) = *arg3;
    *(uint128 *)(p + 0x50) = *arg2;
    *(uint128 *)(p + 0x60) = *(uint128 *)((char *)arg3 + 0x10);
    *(uint128 *)(p + 0x70) = *(uint128 *)((char *)arg2 + 0x10);
    return p + 0x10;
}
