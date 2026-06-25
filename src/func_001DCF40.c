// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Thin registration/setup wrapper: resets via func_001D1F80(self, 0, 1) then
// issues 4 six-arg func_00207F80(self, c0, c1, c2, c3, arg1) calls with fixed
// 16-bit literal id pairs. (arg0=self in s1, arg1 in s0 carried across calls.)
extern void func_001D1F80(int, int, int);
extern void func_00207F80(int, int, int, int, int, int);

void func_001DCF40(int a0, int a1) {
    func_001D1F80(a0, 0, 1);
    func_00207F80(a0, 0x7730, 0x7F10, 0x7770, 0x80D0, a1);
    func_00207F80(a0, 0x7A70, 0x7F90, 0x7AB0, 0x8050, a1);
    func_00207F80(a0, 0x8550, 0x7F90, 0x8590, 0x8050, a1);
    func_00207F80(a0, 0x8890, 0x7F10, 0x88D0, 0x80D0, a1);
}
