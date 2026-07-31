// COMPILER: eegcc
// CFLAGS: -O2
//
// MPEG stream pump. `ctx->0x40` points at the decoder state. The image buffer
// (state->0xD8) must be 64-byte aligned; otherwise the error string D_0026B608
// is reported and -1 returned. Then loop: fetch the next start code with
// func_00106B88() until it is 0 (end) or belongs to the stream we want
// (D_002414AC == state->0xD4) and dispatch on it: 0 = flush/finish,
// 1 = restart all three buffer cursors and decode into buffer 0x94,
// 2 = decode into 0x98, 3/4 = decode into 0x9C. Runs until D_00241B80 goes
// non-zero; once a decode call returns -1 no further start codes are fetched.
extern void func_0010A378(char *fmt, int arg);
extern int func_00106B88(void);
extern int func_0010A140(void *ctx);
extern int func_00109F90(void *ctx, int idx, int buf);

extern int D_00241B80;
extern int D_002414AC;
extern char D_0026B608[];
// The original's switch dispatches through the SHARED, EXTERNAL jump table
// jtbl_0026B650 (the consolidated rodata TU at 0x0026xxxx), which a plain C
// `switch` cannot reference — mwcc/gcc would emit their own LOCAL table and the
// reloc would differ. GNU C's computed goto lets us dispatch through the real
// external table, which byte-matches. `keep` exists only to mark the case
// labels address-taken so gcc keeps those basic blocks reachable from the
// computed jump; it costs 16 bytes of .rodata and is never read.
extern void *jtbl_0026B650[];

int sub_image_buffer_needs_to_be_aligned(void *ctx)
{
    char *st;
    int code;
    int ret;
    int buf;

    static void *const keep[] = { &&case_0, &&case_1, &&case_2, &&case_3 };

    ret = 0;
    code = 1;
    st = *(char **)((char *)ctx + 0x40);
    buf = *(int *)(st + 0xD8);
    *(int *)(st + 0x0) = 0;
    if ((buf & 0x3F) != 0) {
        func_0010A378(D_0026B608, buf);
        return -1;
    }

    D_00241B80 = 0;
    do {
        if (ret != -1) {
            do {
                code = func_00106B88();
            } while (code != 0 && D_002414AC != *(int *)(st + 0xD4));
        }
        if ((unsigned int)code < 5) {
            goto *jtbl_0026B650[code];
        }
        goto next;
    case_0:
        func_0010A140(ctx);
        *(int *)(st + 0x0) = 1;
        goto next;
    case_1:
        *(int *)(st + 0xA8) = 0;
        *(int *)(st + 0xA4) = 0;
        *(int *)(st + 0xA0) = 0;
        ret = func_00109F90(ctx, *(int *)(st + 0xA0), *(int *)(st + 0x94));
        *(int *)(st + 0xA0) = *(int *)(st + 0xA0) + 1;
        goto next;
    case_2:
        ret = func_00109F90(ctx, *(int *)(st + 0xA4), *(int *)(st + 0x98));
        *(int *)(st + 0xA4) = *(int *)(st + 0xA4) + 1;
        goto next;
    case_3:
        ret = func_00109F90(ctx, *(int *)(st + 0xA8), *(int *)(st + 0x9C));
        *(int *)(st + 0xA8) = *(int *)(st + 0xA8) + 1;
    next:
        ;
    } while (D_00241B80 == 0);
    return 1;
}
