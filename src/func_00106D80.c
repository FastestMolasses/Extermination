// NEARMISS func_00106D80  (vram 0x00106D80, 0xB0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.80% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// instruction-scheduling wall: list-scheduler swaps two adjacent independent ops. Expected emits 'sltu; movn; sll; addu' for the ftab index clamp+scale; ee-gcc -O2 emits 'sltu; sll; movn; addu'. Body otherwise byte-identical: the while((v1=func_001066F8(0x20))==0x1B5||v1==0x1B2) rotation kept constants inline (54%->90...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00106AB0(void);
extern int func_00106830(int);
extern int func_00106948(int);
extern int func_001066F8(int);
extern void (*ftab_002411A8[])(void);

void func_00106D80(void) {
    int v1;
    func_00106AB0();
    while ((v1 = func_001066F8(0x20)) == 0x1B5 || v1 == 0x1B2) {
        if (v1 == 0x1B5) {
            unsigned int idx;
            func_00106830(0x20);
            idx = func_00106948(4);
            ftab_002411A8[idx > 0xA ? 0 : idx]();
        } else {
            func_00106830(0x20);
        }
        func_00106AB0();
    }
}
