// NEARMISS func_00107310  (vram 0x00107310, 0xF0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.92% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc forward-branch-likely + list-scheduler wall. Body matches structurally. Divergences at -O2 (best of -O2/-O1/-O0; O1=50.7, O0=0): at the D_002414AC==3 test the original uses a plain branch whose delay slot holds the next constant's upper-half load, while ee-gcc 2.9-991111-01 inverts it into a branch-likely that annuls the slot (rest of the note: docs/NEARMISS.md).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

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
	} else if (D_002414AC == 3) {
		n = D_002414C0 ? D_002414AC : 2;
	} else {
		n = 1;
	}

	for (i = 0; i < n; i++) {
		D_002414E8[i] = func_00106948(0x10);
		func_00106948(1);
		D_002414F8[i] = func_00106948(0x10);
		func_00106948(1);
	}
}
