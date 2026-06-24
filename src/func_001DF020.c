// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Resets/initializes player slot `arg0`: snapshots two 0x20-byte const
// templates (D_002534C0, D_002534E0) into stack scratch buffers, captures the
// slot's table entry *(int *)(D_00275670 + arg0*4 + 0x10) as the return value,
// then runs the standard slot-reset call chain (func_001D1FF0 mode 2,
// func_001D7510/_001D1F80/_001D7000/_001D6DD0, then func_001D7A80(arg0, 0x40,
// &tmplA, &tmplB, arg1) applies the two templates, and func_001D1F20 commits).
// Returns the captured table entry.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 991202
// reschedules the qword-copy/table-load block and caps at ~71%; 2.3.3
// reproduces the exact machine code (objdiff 100.0%).
typedef struct { long long a, b, c, d; } Qw2;
extern Qw2 D_002534C0;
extern Qw2 D_002534E0;
extern int *D_00275670;
extern void func_001D1FF0(int a, int b);
extern void func_001D7510(int a, int b, int c);
extern void func_001D1F80(int a, int b, int c);
extern void func_001D7000(int a, int b);
extern void func_001D6DD0(int a, int b, int c);
extern void func_001D7A80(int a, int b, Qw2 *c, Qw2 *d, int e);
extern void func_001D1F20(int a);

int func_001DF020(int arg0, int arg1) {
    Qw2 sp40, sp60;
    int ret;

    sp40 = D_002534C0;
    sp60 = D_002534E0;
    ret = D_00275670[arg0 + 4];
    func_001D1FF0(arg0, 2);
    func_001D7510(arg0, 0, 0);
    func_001D1F80(arg0, 0, 1);
    func_001D7000(arg0, 0);
    func_001D6DD0(arg0, 0x7000, 0x7900);
    func_001D7A80(arg0, 0x40, &sp40, &sp60, arg1);
    func_001D1F20(arg0);
    return ret;
}
