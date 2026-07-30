// NEARMISS func_0010FE28  (vram 0x0010FE28, 0x200 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.91% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// EXPECTED-SIDE splat symbolization artifact only — NOT a codegen difference. 3 instructions differ, all reloc-name-only: (a) `lui v0, %hi(D_00279448)` / `addiu s1, v0, %lo(D_00279448)` in the .s vs our `%hi/%lo(D_00279440+0x8)` — splat minted a separate data label at 0x279448 for what the compiler...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// IOP loadfile RPC: sceSifLoadModule(path, arg_len, args) with the RPC function
// number supplied by the caller (LF_F_MOD_LOAD / LF_F_MOD_LOAD_ENC).

typedef struct {
    int p;              /* 0x000  in: arg_len / out: result */
    int modres;         /* 0x004 */
    char path[252];     /* 0x008 */
    char args[252];     /* 0x104 */
} LfModuleArg;          /* 0x200 */

extern LfModuleArg D_00279440;
extern unsigned char D_00279640[];

extern int func_0010FAD0(void);
extern int sub__9PsIIlibkernl2000_4(void);
extern char *func_00123418(char *dst, const char *src, unsigned int n);  /* strncpy */
extern int func_0010E8A8(void *bd, int fno, int mode, void *send, int ssize,
                         void *recv, int rsize, void *end_func, void *end_para);
extern void *memcpy(void *dst, const void *src, unsigned int len);       /* constant sizes: expanded inline */
extern void *block_copy(void *dst, const void *src, unsigned int len);   /* the game's out-of-line memcpy */

int func_0010FE28(const char *path, int arg_len, const char *args, int fno)
{
    if (func_0010FAD0() < 0) {
        return -0x10000;
    }
    if (sub__9PsIIlibkernl2000_4() != 0) {
        return -0x10004;
    }

    func_00123418(D_00279440.path, path, 252);
    D_00279440.path[251] = '\0';

    if (args != 0) {
        if (arg_len > 252) {
            memcpy(D_00279440.args, args, 252);
            D_00279440.p = 252;
        } else {
            block_copy(D_00279440.args, args, arg_len);
            D_00279440.p = arg_len;
        }
    } else {
        D_00279440.args[0] = '\0';
        D_00279440.p = 0;
    }

    if (func_0010E8A8(D_00279640, fno, 0, &D_00279440, 0x200, &D_00279440, 4, 0, 0) < 0) {
        return -0x10001;
    }
    return D_00279440.p;
}
