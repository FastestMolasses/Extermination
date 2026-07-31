// COMPILER: eegcc
// CFLAGS: -O2

extern void func_00106AB0(void);
extern int func_001066F8(int);
extern void func_0010A378(char *fmt, int a);
extern void func_00106830(int);
extern int func_00106B18(void);
extern int sub_Invalid_macroblock_address_incre(void);
extern void func_0010A3A8(char *msg);

extern int D_00241408;
extern int D_00241428;
extern int D_0024153C;
extern char D_0026B3F8[];
extern char D_0026B420[];

int sub_slice_start_code_0x_08x_out_of_r(int a0, int *pa, int *pb, int *pc)
{
    int code;
    int n;
    int t;

    D_00241408 = 0;
    func_00106AB0();
    code = func_001066F8(0x20);
    if ((unsigned int)(code - 0x101) >= 0xAF) {
        func_0010A378(D_0026B3F8, code);
        return 2;
    }
    func_00106830(0x20);
    n = func_00106B18();
    t = sub_Invalid_macroblock_address_incre();
    *pb = t;
    if (D_00241408 != 0) {
        func_0010A3A8(D_0026B420);
        return 1;
    }
    *pa = ((n << 7) + (code & 0xFF) - 1) * D_00241428 + t - 1;
    *pb = 1;
    D_0024153C = 1;
    pc[5] = 0;
    pc[4] = 0;
    pc[1] = 0;
    pc[0] = 0;
    pc[7] = 0;
    pc[6] = 0;
    pc[3] = 0;
    pc[2] = 0;
    return 0;
}
