// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Thin registration/setup wrapper: resets via func_001D1F80(self, 0, 1) then
// issues 16 six-arg func_00207F80(self, c0, c1, c2, c3, arg1) calls with fixed
// 16-bit literal id pairs. (arg0=self in s1, arg1 in s0 carried across calls.)
extern void func_001D1F80(int, int, int);
extern void func_00207F80(int, int, int, int, int, int);

void func_001DCD40(int a0, int a1) {
    func_001D1F80(a0, 0, 1);
    func_00207F80(a0, 0x72D0, 0x7BA0, 0x7730, 0x7BF0, a1);
    func_00207F80(a0, 0x72D0, 0x7BF0, 0x7320, 0x7EA0, a1);
    func_00207F80(a0, 0x7440, 0x7CA0, 0x75F0, 0x7CC0, a1);
    func_00207F80(a0, 0x7440, 0x7CC0, 0x7480, 0x7EA0, a1);
    func_00207F80(a0, 0x88D0, 0x7BA0, 0x8D30, 0x7BF0, a1);
    func_00207F80(a0, 0x8CE0, 0x7BF0, 0x8D30, 0x7EA0, a1);
    func_00207F80(a0, 0x8A10, 0x7CA0, 0x8BC0, 0x7CC0, a1);
    func_00207F80(a0, 0x8B80, 0x7CC0, 0x8BC0, 0x7EA0, a1);
    func_00207F80(a0, 0x72D0, 0x83E0, 0x7730, 0x8430, a1);
    func_00207F80(a0, 0x72D0, 0x8140, 0x7320, 0x83E0, a1);
    func_00207F80(a0, 0x7440, 0x8320, 0x75F0, 0x8330, a1);
    func_00207F80(a0, 0x7440, 0x8140, 0x7480, 0x8320, a1);
    func_00207F80(a0, 0x88D0, 0x83E0, 0x8D30, 0x8430, a1);
    func_00207F80(a0, 0x8CE0, 0x8140, 0x8D30, 0x83E0, a1);
    func_00207F80(a0, 0x8A10, 0x8320, 0x8BC0, 0x8330, a1);
    func_00207F80(a0, 0x8B80, 0x8140, 0x8BC0, 0x8320, a1);
}
