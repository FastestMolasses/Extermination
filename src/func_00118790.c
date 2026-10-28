// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.18% NEARMISS).
// Fixes read from the original instructions:
//  - the two leading "= 1" stores are written +0x38 first, then +0x3C (ee-gcc's
//    scheduler emits them in the reverse order);
//  - the op address is an integer sum cursor + base, which gives the original
//    operand order of the add;
//  - each branch computes its table index in its own local, and the matched-path
//    byte read goes through a temp (read before the +0x36 store), so both branches
//    end in the same +0x3A store and share the original register choice.
//
// Bytecode stepper: reads the 5-byte op at global table base D_00281AD4 + cursor
// (p+0x8). Sets +0x38/+0x3C to 1. If op[4] is nonzero and equals the halfword at
// +0x36, clears +0x38, +0x36 and +0x3C. Otherwise the 16-bit index op[3]:op[2] is
// stored at +0x14, the table byte at that index goes to +0x3A, and (op[4] nonzero)
// +0x36 counts up by one. Always advances the cursor by 5 and returns it.

extern unsigned char *D_00281AD4;

int func_00118790(void *arg0)
{
	unsigned char *p = (unsigned char *)arg0;
	unsigned char *base = D_00281AD4;
	int cursor = *(int *)(p + 0x8);
	unsigned char *op = (unsigned char *)(cursor + (int)base);
	int b4;
	int cur36;
	int c;

	*(short *)(p + 0x38) = 1;
	*(short *)(p + 0x3C) = 1;

	b4 = op[4];
	if (b4 != 0) {
		cur36 = *(unsigned short *)(p + 0x36);
		if (cur36 == b4) {
			*(short *)(p + 0x38) = 0;
			*(short *)(p + 0x36) = 0;
			*(short *)(p + 0x3C) = 0;
		} else {
			int v1 = (op[3] << 8) + op[2];
			*(int *)(p + 0x14) = v1;
			c = base[v1];          /* read before the +0x36 store */
			*(short *)(p + 0x36) = cur36 + 1;
			*(short *)(p + 0x3A) = c;
		}
	} else {
		int v2 = op[3] << 8;
		*(int *)(p + 0x14) = v2;
		v2 = v2 | op[2];
		*(int *)(p + 0x14) = v2;
		*(short *)(p + 0x3A) = base[v2];
	}

	*(int *)(p + 0x8) = cursor + 5;
	return cursor + 5;
}
