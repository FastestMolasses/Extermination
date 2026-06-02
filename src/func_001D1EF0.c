// Tear-down sequence: resets the subsystem, posts state (3,1), and clears
// slot 0.
extern void func_001D1C50(void);
extern void func_001D2830(int a, int b);
extern void func_001D1EA0(int a);

void func_001D1EF0(void) {
    func_001D1C50();
    func_001D2830(3, 1);
    func_001D1EA0(0);
}
