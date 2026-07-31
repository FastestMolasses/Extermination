// COMPILER: eegcc
// CFLAGS: -O2
// MPEG group_of_pictures_header() parser: reset the decoder state, bump the
// GOP counter, then read the 25-bit time code + closed_gop/broken_link flags
// and fall through into the extension/user-data scanner.

extern int *D_002412F4;
extern int D_00241198;
extern int D_0024119C;
extern int D_002411A0;
extern int D_0024151C;
extern int D_00241520;
extern int D_00241524;
extern int D_00241528;
extern int D_0024152C;
extern int D_00241530;
extern int D_00241534;

extern int func_00106948(int nbits);
extern void func_00106D80(void);

void func_00107178(void)
{
    int *pa = &D_0024151C;
    int *pb = &D_00241520;
    int *p24 = &D_00241524;
    int *p28 = &D_00241528;
    int *p2C = &D_0024152C;
    int *ctx = D_002412F4;
    int t = D_0024119C + 1;

    *(int *)(ctx[0x10] + 0xE8) = 0;
    D_00241198 = t;
    D_002411A0 = 1;

    *pa = func_00106948(1);
    pa = &D_00241534;
    *pb = func_00106948(5);
    pb = &D_00241530;
    *p24 = func_00106948(6);
    func_00106948(1);
    *p28 = func_00106948(6);
    *p2C = func_00106948(6);
    *pb = func_00106948(1);
    *pa = func_00106948(1);

    func_00106D80();
}
