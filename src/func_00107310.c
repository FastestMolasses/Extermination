// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 88.92% NEARMISS).
// Fixes read from the original instructions:
//  - the "not the first mode" arm tests D_002414AC != 3 first and sets the count to 1,
//    with the D_002414C0 pick in the else arm; the original places the count-1 block
//    before the pick, and the old == 3 order made ee-gcc invert it into a
//    branch-likely (not a compiler wall, just the wrong branch order).
//
// Picks how many entry pairs to fetch from the global mode flags (1, 2 or 3), then
// for each one reads two 16-unit values through func_00106948 into D_002414E8[i]
// and D_002414F8[i], each read followed by a 1-unit call.

extern int D_00241448;
extern int D_002414C0;
extern int D_002414B0;
extern int D_002414AC;
extern int D_002414E8[];
extern int D_002414F8[];

extern int func_00106948(int);

void func_00107310(void) {
	int n;
	int i;

	if (D_00241448 != 0) {
		if (D_002414C0 != 0) {
			n = D_002414B0 ? 3 : 2;
		} else {
			n = 1;
		}
	} else if (D_002414AC != 3) {
		n = 1;
	} else {
		n = D_002414C0 ? D_002414AC : 2;
	}

	for (i = 0; i < n; i++) {
		D_002414E8[i] = func_00106948(0x10);
		func_00106948(1);
		D_002414F8[i] = func_00106948(0x10);
		func_00106948(1);
	}
}
