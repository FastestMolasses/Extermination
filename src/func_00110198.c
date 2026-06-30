// NEARMISS func_00110198  (vram 0x00110198, 0xEC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 75.81% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc arg-passing-ABI + forward-branch-likely wall. Function body is structurally identical (switch on a2 in {0,1,2}: lbu/lhu/lw store of D_00279440 to a1). Two walls: (1) the 9-arg call to func_0010E8A8 -- expected lays the overflow args as stack[0]=0 then t0=0x20,t1=a3,t2=0x20,t3=0, but ee-gcc places arg5 in t0 (a...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00279440;
extern int D_00279640;

extern int func_0010FAD0(int a0, int a1);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int s0, int t0, int t1, int t2, int t3);

int func_00110198(int a0, int a1, int a2) {
	int *p;

	if (func_0010FAD0(a0, a1) < 0) {
		return 0xFFFF0000;
	}
	if (a2 >= 3) {
		return 0xFFFEFFFE;
	}

	p = &D_00279440;
	p[0] = a0;
	p[1] = a2;

	if (func_0010E8A8(&D_00279640, 3, 0, p, 0, 0x20, (int)p, 0x20, 0) < 0) {
		return 0xFFFEFFFF;
	}

	if (a2 == 0) {
		*(unsigned char *)a1 = *(unsigned char *)&D_00279440;
		return 0;
	}
	if (a2 == 1) {
		*(unsigned short *)a1 = *(unsigned short *)&D_00279440;
		return 0;
	}
	if (a2 == 2) {
		*(int *)a1 = D_00279440;
		return 0;
	}
	return 0xFFFEFFFE;
}
