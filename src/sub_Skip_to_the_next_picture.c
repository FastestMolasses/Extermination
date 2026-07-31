// COMPILER: eegcc
// CFLAGS: -O2
// MPEG slice loop: decode macroblocks until the picture ends, then drain the IPU.

extern int D_00241428;
extern int D_0024142C;
extern int D_002414AC;
extern int D_002418F8[];
extern char D_0026B3D8[];

extern int sub_Too_many_macroblocks_in_picture(int a, int b);
extern void func_001063E8(void);
extern int sub_Error_code_detected_BDEC(void);
extern void sub_intra_skip_MB(int flag);
extern void func_0010A3A8(char *msg);

int sub_Skip_to_the_next_picture(int arg)
{
    int n;
    int r;

    n = D_00241428 * D_0024142C;
    D_002418F8[0xA0] = 0;
    D_002418F8[0xA1] = 0;
    n = (D_002414AC != 3) ? (n >> 1) : n;

    do {
        r = sub_Too_many_macroblocks_in_picture(arg, n);
    } while (r == 1 || r == 3);

    func_001063E8();
    if (sub_Error_code_detected_BDEC() == 0) {
        r = 2;
    }

    while ((*(volatile unsigned int *)0x1000D400 >> 8) & 1)
        ;

    if (r == 0) {
        sub_intra_skip_MB(D_002418F8[0xA0] == 0);
    }
    if ((unsigned int)(r - 1) < 2) {
        func_0010A3A8(D_0026B3D8);
    }
    return r == 0;
}
