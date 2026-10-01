// COMPILER: eegcc
// CFLAGS: -O2
// SDK (IOP RPC): forwards to func_001157F0 with command 0x15, (a0, a1, 0).
extern void func_001157F0(int a0, int a1, int a2, int a3);

void func_00119810(int a0, int a1) {
    func_001157F0(0x15, a0, a1, 0);
}
