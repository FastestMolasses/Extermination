// COMPILER: eegcc
// CFLAGS: -O2
// SDK (IOP RPC): forwards to func_001157F0 with command 0x28, (a0, a1, a2).
extern void func_001157F0(int a0, int a1, int a2, int a3);

void func_00119978(int a0, int a1, int a2) {
    func_001157F0(0x28, a0, a1, a2);
}
