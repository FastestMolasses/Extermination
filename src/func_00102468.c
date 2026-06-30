// NEARMISS func_00102468  (vram 0x00102468, 0xC4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 70.76% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Same frame-size stride wall as func_001021B0 (identical spin-wait pattern with mode/timeout args + movz 0x1000000 default). C semantically correct, srl/li idioms match, but mask hoisted into extra callee-saved reg -> frame 0x50 vs expected 0x40. Permuter-class frame-size wall. Best 70.8%.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void func_00122B58(void *);
extern unsigned char D_0026B220;

int func_00102468(volatile int *p, int mode, int timeout)
{
    if (mode == 1) {
        return ((unsigned int)*p >> 8) & 1;
    }

    if (*p & 0x100) {
        if (timeout == 0) {
            timeout = 0x1000000;
        }
        timeout -= 1;
        do {
            if (timeout < 0) {
                func_00122B58(&D_0026B220);
                if (((unsigned int)*p >> 8) & 1) {
                    *p = *p & -0x101;
                }
            }
            timeout -= 1;
        } while (*p & 0x100);
    }

    return 0;
}
