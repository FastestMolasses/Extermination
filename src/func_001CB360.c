// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Four-call sequencer (a0 saved across all calls): func_001C7420(a0,0x3F5,0),
// func_001CB2C0(a0,0x3F3,0), func_001D1F80(0,1,0), and a tail call
// func_001D3F50(*(int*)(a0+0x44)) whose loaded arg fills the jal delay slot.
//
// Built with mwcc 2.3.3, not the pinned 991202: the lone residual under 991202
// is the wall-#13 clean delay-slot nop. 2.3.3 is byte-identical (objdiff 100%).
extern void func_001C7420(int, int, int);
extern void func_001CB2C0(int, int, int);
extern void func_001D1F80(int, int, int);
extern void func_001D3F50(int);

void func_001CB360(int arg0) {
    func_001C7420(arg0, 0x3F5, 0);
    func_001CB2C0(arg0, 0x3F3, 0);
    func_001D1F80(0, 1, 0);
    func_001D3F50(*(int *)(arg0 + 0x44));
}
