// NEARMISS func_0010FC38  (vram 0x0010FC38, 0x1EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.99% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// EXPECTED-SIDE (splat) symbolization artifact — NOT a codegen difference. The single diff instruction is `lui $a1, (0x280000 >> 16)` (bare literal in the .s) vs our `lui $a1, %hi(D_00279640)` (HI16 reloc). %hi(D_00279640) == 0x28, so both encode to 0x3C050028 and the LINKED bytes are identical; .t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// SIF loadfile RPC: sceSifLoadModuleBuffer(ptr, arg_len, args) — fno 6.

typedef struct {
    int p;              /* 0x000  in: module buffer ptr / out: result */
    int arg_len;        /* 0x004 */
    char unused[0xFC];  /* 0x008 */
    char args[0xFC];    /* 0x104 */
} LfModBufArg;          /* 0x200 */

extern LfModBufArg D_00279440;
extern unsigned char D_00279640[];

extern int func_0010FAD0(void);
extern int sub__9PsIIlibkernl2000_4(void);
extern int func_0010E8A8(void *bd, int fno, int mode, void *send, int ssize,
                         void *recv, int rsize, void *end_func, void *end_para);
extern void *memcpy(void *dst, const void *src, unsigned int len);   /* constant sizes: expanded inline */
extern void *block_copy(void *dst, const void *src, unsigned int len); /* the game's out-of-line memcpy */

int func_0010FC38(void *ptr, int arg_len, const char *args)
{
    if (func_0010FAD0() < 0) {
        return -0x10000;
    }
    if (sub__9PsIIlibkernl2000_4() != 0) {
        return -0x10004;
    }

    D_00279440.p = (int)ptr;
    if (args != 0) {
        if (arg_len > 252) {
            memcpy(D_00279440.args, args, 252);
            D_00279440.arg_len = 252;
        } else {
            block_copy(D_00279440.args, args, arg_len);
            D_00279440.arg_len = arg_len;
        }
    } else {
        D_00279440.arg_len = 0;
    }

    if (func_0010E8A8(D_00279640, 6, 0, &D_00279440, 0x200, &D_00279440, 4, 0, 0) < 0) {
        return -0x10001;
    }
    return D_00279440.p;
}
