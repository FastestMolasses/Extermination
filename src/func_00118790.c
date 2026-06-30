// NEARMISS func_00118790  (vram 0x00118790, 0x98 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.61% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc register-allocation + minor store-ordering near-miss. C is semantically correct and instruction-for-instruction the right ops/count: bytecode stepper reading global table base D_00281AD4 + cursor(*(p+8)), op[4] zero/nonzero branch (note op[4] is the compare value via beql/bnel-squash; op[3] is the <<8 high by...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char *D_00281AD4;

int func_00118790(void *arg0)
{
	unsigned char *p = (unsigned char *)arg0;
	unsigned char *base = D_00281AD4;
	int cursor = *(int *)(p + 0x8);
	unsigned char *op = base + cursor;
	int b4;
	int b3;
	int cur36;
	int val;

	*(short *)(p + 0x3C) = 1;
	*(short *)(p + 0x38) = 1;

	b4 = op[4];
	if (b4 != 0) {
		cur36 = *(unsigned short *)(p + 0x36);
		if (cur36 == b4) {
			*(short *)(p + 0x38) = 0;
			*(short *)(p + 0x36) = 0;
			*(short *)(p + 0x3C) = 0;
		} else {
			b3 = op[3];
			val = (b3 << 8) + op[2];
			*(int *)(p + 0x14) = val;
			*(short *)(p + 0x36) = cur36 + 1;
			*(short *)(p + 0x3A) = base[val];
		}
	} else {
		b3 = op[3];
		val = b3 << 8;
		*(int *)(p + 0x14) = val;
		val = val | op[2];
		*(int *)(p + 0x14) = val;
		*(short *)(p + 0x3A) = base[val];
	}

	*(int *)(p + 0x8) = cursor + 5;
	return cursor + 5;
}
