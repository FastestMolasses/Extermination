// COMPILER: eegcc
// CFLAGS: -O2

extern char D_002418F8[];
extern int D_00241408;
extern char D_0026B440[];

extern int sub_slice_start_code_0x_08x_out_of_r(int n, int *mba, int *cnt, int *buf);
extern int sub_Error_code_detected_BDEC(void);
extern int func_001066F8(int id);
extern int sub_Invalid_macroblock_address_incre(void);
extern void func_0010A3A8(char *msg);
extern int sub_Invalid_macroblock_type_code_0(int *p0, int *p1, int *p2, int *buf,
                                             int *b1, int *b2);
extern int sub_skiped_macroblock_in_I_picure_is(int *buf, int *p1, int *b1, int *p0);
extern int sub_Invalid_modion_type_ignored_d(int mba, int cnt, int v0, int v1,
                                             int *buf, int *b1, int *b2);
extern void sub_intra_skip_MB(int slot);

int sub_Too_many_macroblocks_in_picture(int unused, int n)
{
    int buf[8];
    int b1[4];
    int b2[4];
    int mba;
    int cnt;
    int p0;
    int p1;
    int p2;
    int ret;
    int slot;
    char *base;
    char *bp;
    char *bq;

    mba = 0;
    cnt = 0;
    ret = sub_slice_start_code_0x_08x_out_of_r(n, &mba, &cnt, buf);
    if (ret != 0)
        return ret;

    D_00241408 = 0;

    for (;;) {
        if (mba >= n)
            return 0;

        base = D_002418F8;
        *(int *)(base + *(int *)(base + 0x280) * 0x140 + 0x13C) = 0;

        if (sub_Error_code_detected_BDEC() == 0)
            return 2;

        if (cnt == 0) {
            if (func_001066F8(0x17) == 0 || D_00241408 != 0) {
                D_00241408 = 0;
                return 3;
            }
            cnt = sub_Invalid_macroblock_address_incre();
            if (D_00241408 != 0) {
                D_00241408 = 0;
                return 1;
            }
        }

        if (mba >= n) {
            func_0010A3A8(D_0026B440);
            return 2;
        }

        if (cnt == 1) {
            if (sub_Invalid_macroblock_type_code_0(&p0, &p1, &p2, buf, b1, b2) == 0) {
                D_00241408 = 0;
                return 1;
            }
        } else {
            if (sub_skiped_macroblock_in_I_picure_is(buf, &p1, b1, &p0) == 0) {
                D_00241408 = 0;
                return 2;
            }
        }

        if (sub_Invalid_modion_type_ignored_d(mba, cnt, p0, p1, buf, b1, b2) == 0) {
            D_00241408 = 0;
            return 2;
        }

        if (mba != 0) {
            bp = D_002418F8;
            sub_intra_skip_MB(*(int *)(bp + 0x280) ^ 1);
        }

        bq = D_002418F8;
        slot = *(int *)(bq + 0x280);
        mba = mba + 1;
        cnt = cnt - 1;
        *(int *)(bq + 0x280) = slot ^ 1;
    }
}
