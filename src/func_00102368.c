// NEARMISS func_00102368  (vram 0x00102368, 0xF0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 61.92% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc regalloc + dead-loop-codegen wall. Branch-likely actually matched here (our `bnel v1,v0` == expected). Two divergences: (1) the original expanded a degenerate empty `while` body (L1023D0: nop;nop;bnez v0 with v0 always 0) into 3 extra real instrs that ee-gcc 2.9-991111-01 optimizes away differently -- rewritin...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_0026B220;

extern void func_00122B58(void *msg);

void func_00102368(int *p, int a1, int a2) {
	int i;

	if (p[0] & 0x100) {
		i = 0xFFFF;
		do {
			if (i < 0) {
				func_00122B58(&D_0026B220);
				if ((p[0] >> 8) & 1) {
					int v;
					do {
						v = p[0] & ~0x100;
					} while (0);
					p[0] = v;
				}
			}
			i--;
		} while (p[0] & 0x100);
	}

	if (p[4] != -1) {
		p[4] = a1;
	}
	p[8] = a2;
	p[0] = (((p[0] & ~0xD) | 8) & ~2) | 0x100;
}
