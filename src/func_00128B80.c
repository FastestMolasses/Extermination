// NEARMISS func_00128B80  (vram 0x00128B80, 0x84 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.03% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// CW-vs-mwcc branch lowering / dead trailing return-0 block. Body, stores, call, and the conditional 0x34->0x36 copy are byte-identical. The sole residual (both 991202 and 233 score the same 83.03%): the early-out 'if(0x36==0 && D_0081080F==0) return 0' lowers under CW 2.3.1 as a forward 'beqz' int...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_0012E070(char *p);
extern unsigned char D_0081080F;

int func_00128B80(char *arg0, char *arg1) {
    if (*(short *)(arg0 + 0x36) == 0 && D_0081080F == 0) {
        return 0;
    }
    *(unsigned char *)(arg0 + 0) = 3;
    *(unsigned char *)(arg0 + 4) = 2;
    *(unsigned char *)(arg0 + 5) = 0;
    *(unsigned char *)(arg0 + 6) = 0;
    *(unsigned char *)(arg0 + 7) = 0;
    func_0012E070(arg1);
    if (D_0081080F != 0) {
        *(short *)(arg0 + 0x36) = *(short *)(arg0 + 0x34);
    }
    return 1;
}
