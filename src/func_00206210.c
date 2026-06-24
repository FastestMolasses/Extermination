// COMPILER: mwcc233
// CFLAGS: -O4,p
//
// Object/handle initializer. Zeroes several int fields of *arg0 and stores
// arg1->+0x30, arg2->+0x3C, arg3->+0x48, then loads a gp-rel handle
// D_00275B4C into +0x44. If that handle is negative, calls func_00204080 on the
// error string/table D_002734A0 and returns 0. Otherwise func_0011A758(), then
// builds a 4-int command record on the stack {flag, 0x10400, +0x44 value, arg3}
// and submits it twice via func_0011A788 (2nd record: flag=1, value+0x200),
// finally func_0011A830(8) and returns 1.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): 991202 left the idiom-13
// clean-store delay-slot nop; 2.3.3 matches byte-identical. TWO load-bearing
// shapes: (1) the stack command record MUST be a single aggregate `int buf[4]`
// (four separate locals get dead-store-eliminated, collapsing the frame); (2)
// DUAL ADDRESSING -- D_00275B4C is gp-rel (small int) and D_002734A0 is
// absolute, so drop -sdatathreshold 0 and make D_002734A0 a large array.
// objdiff 100% vs build/expected/func_00206210.o.
extern void func_0011A758(void);
extern void func_0011A788(int *);
extern void func_0011A830(int);
extern void func_00204080(void *);
extern int D_002734A0[0x4000];
extern int D_00275B4C;

int func_00206210(char *arg0, int arg1, int arg2, int arg3) {
    int buf[4];

    *(int *)(arg0 + 0) = 0;
    *(int *)(arg0 + 0x2C) = 0;
    *(int *)(arg0 + 0x30) = arg1;
    *(int *)(arg0 + 0x34) = 0;
    *(int *)(arg0 + 0x38) = 0;
    *(int *)(arg0 + 0x3C) = arg2;
    *(int *)(arg0 + 0x40) = 0;
    *(int *)(arg0 + 0x54) = 0;
    *(int *)(arg0 + 0x48) = arg3;
    *(int *)(arg0 + 0x4C) = 0;
    *(int *)(arg0 + 0x50) = 0;
    *(int *)(arg0 + 0x44) = D_00275B4C;
    if (*(int *)(arg0 + 0x44) < 0) {
        func_00204080(D_002734A0);
        return 0;
    }
    func_0011A758();
    buf[0] = 0;
    buf[1] = 0x10400;
    buf[2] = *(int *)(arg0 + 0x44);
    buf[3] = arg3;
    func_0011A788(buf);
    buf[0] = 1;
    buf[1] = 0x10400;
    buf[2] = *(int *)(arg0 + 0x44) + 0x200;
    buf[3] = arg3;
    func_0011A788(buf);
    func_0011A830(8);
    return 1;
}
