// NEARMISS func_001D2090  (vram 0x001D2090, 0x7C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.10% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation-ORDER wall (first half). Residual is 10 DIFF_ARG_MISMATCH only: instruction sequence is byte-exact, but the first packet's loop-invariant pointer/constant registers rotate (base reload lands $a3, target uses $v1; constant temps $v1<->$a2<->$a3). Second half (0x40-0x78) is byte...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 97.1% (mwcc233) -- register-allocation-ORDER wall, first packet.
// Logic fully recovered; second half (0x40-0x78) is byte-perfect. The first
// half's instruction sequence is identical to target but the base-reload and
// constant temps land in rotated registers ($a3 vs target $v1). Idioms applied:
// d674 hoisted to an early local (fixes instruction sequence); a0=0x50 reuses
// the dead index param for the second packet (makes it byte-exact). Remaining
// delta is pure regalloc ordering -> permuter.
extern int *D_00275670;
extern int D_00275674;
void func_001D2090(int a0, int a1) {
    char *p;
    int *t0;
    int d674 = D_00275674;
    t0 = D_00275670 + a0;
    p=(char*)t0[4]; *(char*)(p+3)=0x30;
    p=(char*)t0[4]; *(int*)(p+4)=d674;
    p=(char*)t0[4]; *(short*)(p+0)=1;
    p=(char*)t0[4]; t0[4]=(int)(p+0x10);
    D_00275670[a0+0x14]=a1;
    t0=D_00275670+a0; a0=0x50;
    p=(char*)t0[4]; *(char*)(p+3)=a0;
    p=(char*)t0[4]; *(int*)(p+4)=a1;
    p=(char*)t0[4]; *(short*)(p+0)=0;
    p=(char*)t0[4]; t0[4]=(int)(p+0x10);
}
