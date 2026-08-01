// COMPILER: eegcc
// CFLAGS: -O2

// IOP loadfile RPC: sceSifLoadModule(path, arg_len, args) with the RPC function
// number supplied by the caller (LF_F_MOD_LOAD / LF_F_MOD_LOAD_ENC).
//
// D_00279448 and D_00279440 are the SAME object: D_00279448 is splat's label for
// D_00279440.path (base + 8), minted because the original ELF is stripped and splat
// symbolizes by address, so it cannot see the reloc was symbol+addend. Declaring both
// names and deriving the struct base as (D_00279448 - 8) reproduces the original
// reloc set exactly: %hi/%lo(D_00279448) for the strncpy destination (with the base
// CSE'd as `addiu v0, s1, -8`), and %hi/%lo(D_00279440) for the three standalone
// materializations. Verified .text byte-identical (512/512 bytes, cmp clean).

typedef struct {
    int p;              /* 0x000  in: arg_len / out: result */
    int modres;         /* 0x004 */
    char path[252];     /* 0x008 */
    char args[252];     /* 0x104 */
} LfModuleArg;          /* 0x200 */

extern char D_00279448[252];        /* == D_00279440.path */
extern LfModuleArg D_00279440;
extern unsigned char D_00279640[];

#define LF (*(LfModuleArg *)(D_00279448 - 8))

extern int func_0010FAD0(void);
extern int sub__9PsIIlibkernl2000_4(void);
extern char *func_00123418(char *dst, const char *src, unsigned int n);  /* strncpy */
extern int func_0010E8A8(void *bd, int fno, int mode, void *send, int ssize,
                         void *recv, int rsize, void *end_func, void *end_para);
extern void *memcpy(void *dst, const void *src, unsigned int len);       /* constant size: expanded inline */
extern void *block_copy(void *dst, const void *src, unsigned int len);   /* the game's out-of-line memcpy */

int func_0010FE28(const char *path, int arg_len, const char *args, int fno)
{
    if (func_0010FAD0() < 0) {
        return -0x10000;
    }
    if (sub__9PsIIlibkernl2000_4() != 0) {
        return -0x10004;
    }

    func_00123418(D_00279448, path, 252);
    LF.path[251] = '\0';

    if (args != 0) {
        if (arg_len > 252) {
            memcpy(LF.args, args, 252);
            D_00279440.p = 252;
        } else {
            block_copy(LF.args, args, arg_len);
            LF.p = arg_len;
        }
    } else {
        LF.args[0] = '\0';
        LF.p = 0;
    }

    if (func_0010E8A8(D_00279640, fno, 0, &D_00279440, 0x200, &D_00279440, 4, 0, 0) < 0) {
        return -0x10001;
    }
    return D_00279440.p;
}
