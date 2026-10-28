// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.33% NEARMISS).
// Fixes read from the original instructions:
// - The request word at +0x10 is a pointer field (the sibling func_001108E8
//   stores a buffer pointer there), so it is cleared as a pointer, not an int.
//   With strict aliasing the pointer store no longer depends on the int store at
//   +0, which lets the scheduler set up the request pointer first, as the
//   original does.
// - The two 4-entry tables at D_00279750 are written as a struct array instead
//   of byte offsets (same code, readable), with the callee prototype taken from
//   the sibling file.
//
// Initialises the RPC client: clears the first three words of every entry of
// the two 4-entry tables (stride 0x60) at D_00279750, builds a request in
// D_00279A80 (command 0x10, null buffer), issues RPC call 1 through
// func_0010E8A8 with the request as both send and receive buffer (0x80 bytes
// each), and returns the reply word at +0xC, or 0 if the call failed.

typedef struct { int a; int b; int c; int pad[21]; } Slot;
extern Slot D_00279750[2][4];
extern unsigned char D_00279A80[];
extern unsigned char D_00279700[];
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, int a7, int a8);

int func_001107C8(void)
{
	unsigned char *ctl;
	int i;

	for (i = 0; i < 4; i++) {
		D_00279750[0][i].a = 0;
		D_00279750[0][i].c = 0;
		D_00279750[0][i].b = 0;
		D_00279750[1][i].a = 0;
		D_00279750[1][i].c = 0;
		D_00279750[1][i].b = 0;
	}

	ctl = D_00279A80;
	*(int *)ctl = 0x10;
	*(unsigned char **)(ctl + 0x10) = 0;

	if (func_0010E8A8(D_00279700, 1, 0, ctl, 0x80, ctl, 0x80, 0, 0) < 0) {
		return 0;
	}
	return *(int *)(ctl + 0xC);
}
