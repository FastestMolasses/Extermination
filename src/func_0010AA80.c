// NEARMISS func_0010AA80  (vram 0x0010AA80, 0xC0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.19% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Logic/frame(0x60)/externs all correct; return type void confirmed (no trailing move v0,0). Residual diff: the two volatile MMIO stores *(0x10002000)=0 and *(0x1000B400)=0x101 do not get scheduled into the preceding jal func_001063E8 / jal func_001063B8 delay slots (gcc emits nop and places the store before the call)...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00109B20(int a0, void *a1);
extern int func_001063E8(void);
extern int func_001063B8(int a0);
extern int D_002412F4;

void func_0010AA80(int a0, int a1) {
    int local[8];

    local[0] = 2;
    func_00109B20(D_002412F4, local);
    func_001063E8();
    *(volatile int *)0x10002000 = 0;
    func_001063E8();
    *(volatile int *)0x1000B410 = a1 & 0xFFFFFFF;
    *(volatile int *)0x1000B420 = 4;
    *(volatile int *)0x1000B400 = 0x101;
    func_001063B8(a0);
    func_001063E8();
    local[0] = 3;
    func_00109B20(D_002412F4, local);
}
