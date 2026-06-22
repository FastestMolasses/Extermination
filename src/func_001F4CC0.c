// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Sets up three audio/effect parameter ranges via func_0021B9A0(slot, lo, hi):
// slot 2 -> [0, 100000], slot 3 -> [0, 1000000], then forwards (a0,a1) to
// func_001F4BF0, and finally slot 1 -> [0, 0].
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not 991202: under 991202 the
// s0/s1 saves and the f12 mtc1 are scheduled/placed differently (73.3%);
// 2.3.3 reproduces the target ordering -> 100%.
extern void func_0021B9A0(int, float, float);
extern void func_001F4BF0(int, int);

void func_001F4CC0(int a0, int a1) {
    func_0021B9A0(2, 0.0f, 100000.0f);
    func_0021B9A0(3, 0.0f, 1000000.0f);
    func_001F4BF0(a0, a1);
    func_0021B9A0(1, 0.0f, 0.0f);
}
