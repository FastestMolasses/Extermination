// NEARMISS func_00107A28  (vram 0x00107A28, 0x9C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.77% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc branch-delay-slot fill choice. C is structurally correct and bytes match except one block: a bounds-check wrapper reading (*(D_002412F4+0x40)), comparing fields 0xDC/0xE4 against arg0[1]/arg0[2]/(arg0[3]*arg0[4]) into bool s0, then if !s0 calls sprintf-like func_00122D10(buf,D_0026B548,arg0[1],arg0[2]) + func...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void *D_002412F4;
extern char D_0026B548[];
extern int func_00122D10(char *buf, char *fmt, int a, int b);
extern int func_0010A3A8(char *buf);

int func_00107A28(int *arg0)
{
	char buf[0x100];
	int *ctx = (int *)(*(int *)((char *)D_002412F4 + 0x40));
	int e0 = ctx[0xE0 / 4];
	int s0;

	if (e0 != 0) {
		if (*(int *)((char *)ctx + 0xDC) < arg0[1]) {
			s0 = 0;
		} else {
			s0 = !(e0 < arg0[2]);
		}
	} else {
		s0 = !(*(int *)((char *)ctx + 0xE4) < (arg0[3] * arg0[4]));
	}

	if (s0 == 0) {
		func_00122D10(buf, D_0026B548, arg0[1], arg0[2]);
		func_0010A3A8(buf);
	}
	return s0;
}
