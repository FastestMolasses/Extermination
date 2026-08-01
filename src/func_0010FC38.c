// COMPILER: eegcc
// CFLAGS: -O2
//
// BYTE-MATCHED (objdiff 100.0). This function sat at 99.9919% for two sessions on
// a single instruction, recorded as an unfixable compiler artifact. It was neither
// unfixable nor a compiler artifact: splat had failed to pair one HOISTED %hi with
// its %lo partner (they sit ~75 instructions apart, across the copy loops) and
// emitted a bare `lui $a1, (0x280000 >> 16)` instead of `%hi(D_00279640)`. The two
// nearby %hi references to the SAME symbol were paired correctly, which is what
// made the residual look like codegen.
//
// Both forms assemble to the identical word, so the linked binary never differed —
// only objdiff's relocation comparison did. Fixed in the EXPECTED object by
// build.py's _symbolize_hoisted_hi() pass, not by contorting this source: emitting
// a bare literal here would have fixed the one site and broken the two that splat
// got right.

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
