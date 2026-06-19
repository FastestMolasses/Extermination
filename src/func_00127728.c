// COMPILER: eegcc
// CFLAGS: -O2
typedef struct {
    int a;
    int b;
    int c;
    int _pad;
    long long d;
} Args127728;

extern void func_00126AB8(Args127728 *p);

void func_00127728(int a0, int a1, int a2, long long a3) {
    Args127728 args;
    args.a = a0;
    args.b = a1;
    args.c = a2;
    args.d = a3;
    func_00126AB8(&args);
}
