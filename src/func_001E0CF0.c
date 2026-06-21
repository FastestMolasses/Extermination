// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Init guarded by func_001D2910(state) gates. Outer guard 0x20; inner guards
// 0x21 and 0x22 each register a sub-object into D_00275670 (offsets 0x1D8/0x1E8)
// via func_001E1E60 / func_001E1AD0. The pinned 991202 build only reaches 87.5%
// because it will not hoist the `addiu a0,0x22` into the beqz delay slot; mwcc
// 2.3.3 schedules it identically to CW -> 100.0 byte-identical.
extern int func_001E0CC0(void);
extern int func_001D2910(int);
extern int func_001E1E60(int, int);
extern int func_001E1AD0(int, int);
extern int *D_00275670;

void func_001E0CF0(void) {
    func_001E0CC0();
    if (func_001D2910(0x20)) {
        if (func_001D2910(0x21)) {
            D_00275670[0x1D8 / 4] = func_001E1E60((int)D_00275670 + 0x180, 3);
        }
        if (func_001D2910(0x22)) {
            D_00275670[0x1E8 / 4] = func_001E1AD0((int)D_00275670 + 0x1E0, 3);
        }
    }
}
