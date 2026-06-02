// Resolve a clip id from the entity's variant byte, then commit it with the
// caller-supplied blend value.
extern short func_0017B490(unsigned char *e, int a1, int variant, int a3);
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);

void func_00174A50(unsigned char *e, float blend) {
    short clip = func_0017B490(e, 0, e[0x235], 0);
    func_001749A0(e, clip, 0, blend);
}
