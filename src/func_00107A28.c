// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.77% NEARMISS).
// Fixes read from the original instructions:
//  - The bounds result is a short-circuit `&&` expression (field 0xDC >= arg0[1]
//    && field 0xE0 >= arg0[2]), not an if/else with an explicit `s0 = 0` arm and a
//    negated compare. The nested if/else form let ee-gcc cross-jump the two
//    identical "invert the compare" tails into one; the original keeps both copies.
//
// Bounds check: reads the context at (*(D_002412F4 + 0x40)). If its field 0xE0 is
// nonzero, the request (arg0[1], arg0[2]) must fit within fields 0xDC / 0xE0;
// otherwise the area arg0[3] * arg0[4] must fit within field 0xE4. On failure it
// formats a message (sprintf-like func_00122D10 with D_0026B548, arg0[1], arg0[2])
// and prints it via func_0010A3A8. Returns 1 when the request fits, else 0.

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
		s0 = *(int *)((char *)ctx + 0xDC) >= arg0[1] && e0 >= arg0[2];
	} else {
		s0 = *(int *)((char *)ctx + 0xE4) >= arg0[3] * arg0[4];
	}

	if (s0 == 0) {
		func_00122D10(buf, D_0026B548, arg0[1], arg0[2]);
		func_0010A3A8(buf);
	}
	return s0;
}
