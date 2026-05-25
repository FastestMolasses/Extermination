// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001D1AE0(void);
extern char D_007635C0[64];  // sized to exceed sdata threshold so mwcc uses hi/lo not gp_rel
extern void func_001CB5C0(int *);
extern void func_001F0310(void);
extern void func_001D2830(int, int);

void func_001D1C10(void) {
    func_001D1AE0();
    func_001CB5C0((int*)D_007635C0);
    func_001F0310();
    func_001D2830(4, 1);
}
