// COMPILER: eegcc
// CFLAGS: -O2
// MPEG macroblock_address_increment decoder: VLC 0x22 = macroblock_stuffing,
// 0x23 = macroblock_escape (+33); code 0 means a bad VLC, which is tolerated
// once for an MPEG-1 stuffing pattern and otherwise reported as an error.

extern int D_00241408;
extern int D_00241BB0;
extern char D_0026B3A0[];

extern int func_00106540(int);
extern int func_001066F8(int nbits);
extern void func_00106830(int nbits);
extern void func_0010A378(char *msg, int code);

int sub_Invalid_macroblock_address_incre(void)
{
    int inc = 0;
    int again;
    int code;

    do {
        code = func_00106540(0);
        switch ((unsigned int)code) {
        case 0x22:
            again = 1;
            break;
        case 0x23:
            again = 1;
            inc += 0x21;
            break;
        case 0:
            {
                int peek = func_001066F8(0xB);
                if (D_00241BB0 != 0 && peek == 0xF) {
                    func_00106830(0xB);
                    again = 1;
                } else {
                    func_0010A378(D_0026B3A0, code);
                    D_00241408 = 1;
                    return 1;
                }
            }
            break;
        default:
            inc += code;
            again = 0;
            break;
        }
    } while (again);

    return inc;
}
