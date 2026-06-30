// NEARMISS func_001107C8  (vram 0x001107C8, 0x9C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.33% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc list-scheduler artifact, NOT a permuter-crackable reg-perm. C is structurally correct (init loop over D_00279750 stride 0x60 x4 elems clearing +0/4/8/0x180/0x184/0x188, then D_00279A80=0x10, q[4]=0, 9-arg call to func_0010E8A8, bgezl-guarded return q[3]). Only diff is a single instruction-order swap: expected...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00279750;
extern int D_00279A80;
extern int D_00279700;
extern int func_0010E8A8(int a0, int a1, int a2, int *a3, int t0, int *t1, int t2, int t3, int sp0);

int func_001107C8(void)
{
	char *p = (char *)&D_00279750;
	char *end = (char *)&D_00279750 + 0x180;
	int *q;
	int rv;

	do {
		*(int *)(p + 0x0) = 0;
		*(int *)(p + 0x8) = 0;
		*(int *)(p + 0x4) = 0;
		*(int *)(p + 0x180) = 0;
		*(int *)(p + 0x188) = 0;
		*(int *)(p + 0x184) = 0;
		p += 0x60;
	} while ((int)p < (int)end);

	q = &D_00279A80;
	q[0] = 0x10;
	q[4] = 0;

	rv = func_0010E8A8((int)&D_00279700, 1, 0, q, 0x80, q, 0x80, 0, 0);
	if (rv < 0)
		return 0;
	return *(int *)((char *)q + 0xC);
}
