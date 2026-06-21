// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Multi-call non-leaf: func_001AEDE0(p, 0) then 3x func_001FAD70(i, p, 1).
// First callee must be declared with 2 args (not 3) or mwcc hoists the
// shared `li a2,1` constant before the first call, breaking the match.
extern void func_001AEDE0(int, int);
extern void func_001FAD70(int, int, int);

void func_001B0C00(int p) {
    func_001AEDE0(p, 0);
    func_001FAD70(0, p, 1);
    func_001FAD70(1, p, 1);
    func_001FAD70(2, p, 1);
}
