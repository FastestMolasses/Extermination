// NEARMISS func_00119240  (vram 0x00119240, 0x90 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.36% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Frame-size / reg-alloc wall (off by 1 instr). Calls DisableDmacHandler(0), func_0010E088(0), then loops: func_0010E6F8(&D_00281B00, 0x736E646E, 0); if <0 return -1; spin-delay 0x2710; repeat while *(int*)(D_00281B00+0x24)==0; return 0. Semantics correct. Expected uses TWO callee-saved regs (s0=%hi(D_00281B00), s1=fu...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int DisableDmacHandler(int chan);
extern int func_0010E088(int arg);
extern int func_0010E6F8(void *p, unsigned int magic, int arg);

extern unsigned char D_00281B00[];

int func_00119240(void) {
    int v0;

    DisableDmacHandler(0);
    func_0010E088(0);

    do {
        if (func_0010E6F8(&D_00281B00, 0x736E646E, 0) < 0) {
            return -1;
        }
        v0 = 0x2710;
        do {
            v0--;
        } while (v0 > 0);
    } while (*(int *)(&D_00281B00[0x24]) == 0);

    return 0;
}
