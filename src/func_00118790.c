// NEARMISS func_00118790  (vram 0x00118790, 0x98 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.18% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc register-allocation + minor store-ordering near-miss. C is semantically correct and instruction-for-instruction the right ops/count: bytecode stepper reading global table base D_00281AD4 + cursor(*(p+8)), op[4] zero/nonzero branch (note op[4] is the compare value via beql/bnel-squash; op[3] is the <<8 high by...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// Corrected 2026-10-01 against the original instructions (docs/FINDINGS.md "NEARMISS body
// corrections from the level side-track lanes"): on the path where op[4] is nonzero and differs
// from +0x36, the byte base[v] is read before +0x36 = k + 1 is stored.
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
			b4 = base[val];          /* read before the +0x36 store */
			*(short *)(p + 0x36) = cur36 + 1;
			*(short *)(p + 0x3A) = b4;
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
